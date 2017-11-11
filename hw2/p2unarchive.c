#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <utime.h>
#include <stdlib.h>
#include <dirent.h>

#define BYTES_PARTS 512
#define FAILURE -1
#define MAX_PATH_LEN 255
#define MAX_NAME_LEN 255
#define END_OF_FILE 13
#define DESIRED_ARGS 2

int my_write(int fd, const void *buffer,int count,int line){
	int bytes_written;

    bytes_written = (int) write(fd, buffer, (size_t) count);

    if(bytes_written == FAILURE){
        fprintf(stderr, "write error / line %d\n",line);
		perror("");
        close(fd);
        exit(EXIT_FAILURE);
    }
    else
        return(bytes_written);
}

int my_read(int fd, void *buf, int nbytes, int line){
    int bytes_read;

    bytes_read = (int) read(fd,buf, (size_t) nbytes);

    if(bytes_read == FAILURE){
        fprintf(stderr,"Read error on line %d\n",line);
        perror("");
        close(fd);
        exit(EXIT_FAILURE);
    }
    else{
        if(!bytes_read){
            return(END_OF_FILE);
        }
        else
            return(bytes_read);
    }
}

void copy_specific_bytes(int fd_archived,int fd_file, int size_file){
	int i, bytes_read, bytes_copied;
	char buffer[BYTES_PARTS];

	bytes_copied = 0;

	for (i=0; i<=size_file/BYTES_PARTS; i++){
		if (size_file - bytes_copied < BYTES_PARTS){
			bytes_read=my_read(fd_archived,buffer,size_file-bytes_copied, __LINE__);
			my_write(fd_file, buffer, bytes_read,__LINE__);
			break;
		}
  		bytes_read=my_read(fd_archived, buffer, BYTES_PARTS,__LINE__);
		bytes_copied += bytes_read;

  	my_write(fd_file, buffer, bytes_read,__LINE__);
	}
}

void argc_check(int argc){
	if(argc != DESIRED_ARGS ){
	  fprintf (stderr,"Not the right number of arguments. Terminating...\n");
	  exit(EXIT_FAILURE);
	}
}

int main (int argc,char *argv[]){

	int fd_file;
	size_t size_of_name;
	char name_of_file[MAX_NAME_LEN + 1],path[MAX_PATH_LEN + 1];
	struct utimbuf change_time;
	mode_t mode;
	off_t size_of_file;

  	argc_check(argc);
	//Creating the directory.
	if(mkdir(argv[1], S_IRWXU) == -1){
		perror("mkdir_unarchive");
		return (1);
	}

	if(chdir(argv[1]) == -1){
		perror("chdir_unarchive");
		return (1);
	}

	getcwd(path, sizeof(path));

	while(1){

		if(my_read(STDIN_FILENO, &size_of_name, sizeof(size_t), __LINE__) == END_OF_FILE){
			break;
		}

		my_read(STDIN_FILENO, name_of_file, size_of_name, __LINE__);

		my_write(2, &size_of_name, sizeof(size_t), __LINE__);
		name_of_file[(int)size_of_name] = '\0';

		fd_file = open(name_of_file, O_CREAT | O_RDWR, S_IRWXU);
		if(fd_file == -1){
			perror("open_unarchive");
			return (1);
		}

		my_read(STDIN_FILENO, &(change_time.actime), sizeof(time_t),__LINE__);
		
		my_read(STDIN_FILENO, &(change_time.modtime), sizeof(time_t), __LINE__);

		my_read(STDIN_FILENO, &mode, sizeof(mode_t), __LINE__);

		if(fchmod(fd_file, mode) == -1){
			perror("fchmod_unarchive");
			return (1);
		}
		my_read(STDIN_FILENO, &size_of_file, sizeof(off_t), __LINE__);

		copy_specific_bytes(STDIN_FILENO, fd_file, (int)size_of_file);

		if(utime(name_of_file, &change_time) == -1){
			perror("utime_unarchive");
			return (1);
		}
		close(fd_file);
	}

	return (0);
}
