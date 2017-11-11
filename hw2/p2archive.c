/* Olympia Axelou 2161 Vassilis Samaras 2144 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_PATH_LENGTH 255
#define END_OF_FILE 13
#define FAILURE -1
#define BYTES_PARTS 512

int my_write(int fd, const void *buffer,int count,int line){
	int bytes_written;

    bytes_written = (int) write(fd, buffer, (size_t) count);

    if(bytes_written == FAILURE){
        fprintf(stderr,"write error / line%d \n",line);
		perror("");
        close(fd);
        exit(EXIT_FAILURE);
    }
    else{
		return(bytes_written);
	}

}

int my_open(const char *pathname,int flags,mode_t mode){
	int fd;

    fd = open(pathname,flags,mode);

    if(fd == FAILURE ){
      perror("open");
      exit(EXIT_FAILURE);
    }
	else{
		return(fd);
	}

}

char *absolute_path( char *full_directory_path ){
	char *getcwd_check;

    getcwd_check = getcwd(full_directory_path, MAX_PATH_LENGTH);
    if(getcwd_check == NULL){
        perror("getcwd_p2archive");
        exit(EXIT_FAILURE);
    }
	else{
		return(full_directory_path);
	}

}


int my_read(int fd, void *buf, int nbytes, int line){
	int bytes_read;

    bytes_read = (int) read(fd,buf, (size_t) nbytes);

    if(bytes_read == FAILURE){
        fprintf(stderr, "read error on line %d\n",line);
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

int main (int argc,char *argv[]){
	char *dir_name; char *last_slash;
  	char name_without_path[MAX_PATH_LENGTH + 1],buffer[BYTES_PARTS];
	int stat_check,fd_file,bytes_read;
  	off_t size_of_obj;
	mode_t object_protect;
  	struct timespec last_access,last_modification;
	size_t name_len;
	struct stat get_info_obj;

	while (1) {
		scanf(" %ms\n",&dir_name);//allocates exactly as much memory as we need
		//for the directory name.
        if(dir_name==NULL){
            break;
        }

		last_slash = strrchr(dir_name,'/');//we find the last slash in dir_name
		if(last_slash == NULL){
			perror("strrchr");
			exit(EXIT_FAILURE);
		}

      stat_check=stat(dir_name,&get_info_obj);
      if(stat_check == FAILURE){
          perror("stat_archive");
          exit(EXIT_FAILURE);
        }

        name_len = strlen(last_slash + 1); //mikos onomatos arxeiou

        strcpy(name_without_path,last_slash + 1); //onoma se ascii xwris path

        last_access = get_info_obj.st_atim; //teleutaia prosvasi

        last_modification = get_info_obj.st_mtim; //teleutaio modification

        object_protect = get_info_obj.st_mode; //dikaiwmata kai eidos arxeiou

        size_of_obj = get_info_obj.st_size; //mikos arxeiou - total size

        my_write(STDOUT_FILENO,&name_len,sizeof(size_t),__LINE__);
        my_write(STDOUT_FILENO,name_without_path,name_len * sizeof(char),__LINE__);
        my_write(STDOUT_FILENO,&(last_access.tv_sec),sizeof(time_t),__LINE__);
        my_write(STDOUT_FILENO,&(last_modification.tv_sec),sizeof(time_t),__LINE__);
        my_write(STDOUT_FILENO,&object_protect,sizeof(mode_t),__LINE__);
        my_write(STDOUT_FILENO,&size_of_obj,sizeof(off_t),__LINE__);

        fd_file = my_open(dir_name,O_RDONLY,0);

        while(1){
    		if((bytes_read=my_read(fd_file, buffer, BYTES_PARTS, __LINE__)) == END_OF_FILE){
    		break;
    		}
    		my_write(STDOUT_FILENO, buffer, bytes_read, __LINE__);
    	}

		free(dir_name);
        close(fd_file);
    }

    return(0);
}
