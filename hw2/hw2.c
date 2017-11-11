/* Olympia Axelou 2161 Vassilis Samaras 2144 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAGIC_NUMBER_LEN 9
#define ZERO 0
#define DECRYPT_CHOSEN 911
#define ENCRYPT_CHOSEN 666
#define DESIRED_ARGS 5

void encrypt (char *directory_name, char *encrypt_key, char *file_name);
void decrypt (char *directory_name, char *encrypt_key, char *file_name);

//The following function examines if the file is valid for our programm.
void parse_magic(int filedescr){
	char magic_number[]="P2CRYPTAR";
	char file_checker[MAGIC_NUMBER_LEN];
	ssize_t bytes_read;
	int cmp_check;

	bytes_read = read(filedescr,file_checker,MAGIC_NUMBER_LEN);

	if(bytes_read == ZERO){
		fprintf(stderr,"The file is 100 percent empty. It does not contain any data.\n");
		exit(EXIT_FAILURE);
	}
	else{
		if(bytes_read < ZERO){
			perror("read ");
			exit(EXIT_FAILURE);
		}
	}
	// if we manage to reach the next line it means that we read the first 9
	//bytes and we are going to test if the object is valid for our programm
	cmp_check = memcmp(file_checker,magic_number,MAGIC_NUMBER_LEN);
	if(cmp_check != ZERO){
		fprintf(stderr, "Not an archived file. Terminating...\n");
		exit(EXIT_FAILURE);
	}
}

void write_magic(int filedescr){
	char magic_number[]="P2CRYPTAR";
	ssize_t bytes_written;

	bytes_written = write(filedescr, magic_number, MAGIC_NUMBER_LEN);
	if(bytes_written == -1){
		perror("write (write_magic)");
		exit(EXIT_FAILURE);
	}
	//else magic number was written
}

void argc_check(int argc){
	if( argc != DESIRED_ARGS ){
	  fprintf (stderr,"Not the right number of arguments. Terminating...\n");
	  exit(EXIT_FAILURE);
	}
}

void print_menu(){
	fprintf(stderr,"\n ================== \n");
	fprintf(stderr," You inserted a wrong flag......\n");
	sleep(2);
	fprintf(stderr," HERE COMES THE BOOOOOOOOM\n");
	sleep(1);
	fprintf(stderr," Dear user, we would like to inform you that we accept as a second argument:\n");
	fprintf(stderr," \"-E\" Which will archive and encrypt your desired data.\n");
	fprintf(stderr," OR...\n");
	fprintf(stderr," \"-D\" Which will unarchive and decrypt your already archived and encrypted data.\n");
	fprintf(stderr," We hope that we will provide you a unique experience.\n");
	fprintf(stderr," Thank you for using our software.\n");
	fprintf(stderr,"                                  :)\n");
	fprintf(stderr,"\n ================== \n\n");
}
/* The following function checks the second argument that the user inserted.
These are the instructions that we got from our professors. IF the user chooses
Decryption we return the DECRYPT_CHOSEN value. Else we return the ENCRYPT_CHOSEN
value.*/
int user_interaction_check(char *argv[]){

	if ( ( strcmp(argv[1],"-D") ) == ZERO){
		return(DECRYPT_CHOSEN);
	}
	else{
		if ( ( strcmp(argv[1],"-E") ) == ZERO){
			return(ENCRYPT_CHOSEN);
		}
		else{
			print_menu();
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc,char *argv[]){
	int desired_action;

	argc_check(argc);
	desired_action = user_interaction_check(argv);

	switch (desired_action) {
		case DECRYPT_CHOSEN:{
			decrypt(argv[2],argv[3],argv[4]);
			break;
		}
		case ENCRYPT_CHOSEN:{
			encrypt(argv[2],argv[3],argv[4]);
			break;
		}
	}

	return (0);
}

/* Encrypt creates an object (c.c.file_name) which will contain the data that exist in the file
that the user inserted as an argument (c.c. directory_name)*/
void encrypt (char *directory_name, char *encrypt_key, char *file_name){
	int pipe_dirlist_archive[2], pipe_archive_crypt[2];
	int file_fd, status, i;
	pid_t pid_dirlist, pid_archive, pid_crypt;

	file_fd = open(file_name, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);
	if(file_fd == -1){
		perror("open_encrypt");
		exit(EXIT_FAILURE);
	}

	write_magic(file_fd);

	if(pipe(pipe_dirlist_archive) == -1){
		perror("pipe_dirlist_archive");
		exit(EXIT_FAILURE);
	}

	pid_dirlist = fork();
	if(pid_dirlist == -1){
		perror("fork_dirlist");
		exit(EXIT_FAILURE);
	}

	if (pid_dirlist == 0) {
		close(pipe_dirlist_archive[0]);
		dup2(pipe_dirlist_archive[1], 1);
		close(pipe_dirlist_archive[1]);

		execlp("./dirlist", "dirlist", directory_name, NULL);
		/*if we manage to reach line 157 it means that execlp has failed.
		so we must print an error message*/
		perror("execlp_dirlist");
		exit(EXIT_FAILURE);
	}
	close(pipe_dirlist_archive[1]);

	if(pipe(pipe_archive_crypt) == -1){
		perror("pipe_archive_crypt");
		exit(EXIT_FAILURE);
	}

	pid_archive = fork();
	if(pid_archive == -1){
		perror("fork_archive");
		exit(EXIT_FAILURE);
	}

	if(pid_archive == 0){
		close(pipe_archive_crypt[0]);
		dup2(pipe_dirlist_archive[0],0);
		close(pipe_dirlist_archive[0]);
		dup2(pipe_archive_crypt[1],1);
		close(pipe_archive_crypt[1]);

		execlp("./p2archive", "archive", NULL);
		/*if we manage to reach line 183 it means that execlp has failed.
		so we must print an error message*/
		perror("execlp_archive");
		exit(EXIT_FAILURE);
	}
	close(pipe_dirlist_archive[0]);
	close(pipe_archive_crypt[1]);

	pid_crypt = fork();
	if(pid_crypt == -1){
		perror("fork_crypt");
		exit(EXIT_FAILURE);
	}

	if (pid_crypt == 0){
		dup2(pipe_archive_crypt[0],0);
		close(pipe_archive_crypt[0]);
		dup2(file_fd, 1);

		execlp("./p2crypt", "p2crypt", encrypt_key, NULL);
		/*if we manage to reach line 203 it means that execlp has failed.
		so we must print an error message*/
		perror("execlp_crypt");
		exit(EXIT_FAILURE);
	}
	close(pipe_archive_crypt[0]);
	close(file_fd);

	for (i=0; i<3; i++){
		waitpid(-1, &status, 0);
		if(WIFEXITED(status)){
			if (WEXITSTATUS(status) == 1){
				//child returned 1 (error)
				exit(EXIT_FAILURE);
			}
		}
	}
}
/*Τhe following function will create a directory that will contain the objects
we archived before*/
void decrypt (char *directory_name, char *encrypt_key, char *file_name){
	int pipe_crypt_unarchive[2];
	int file_fd, i, status;
	pid_t pid_unarchive, pid_crypt;

	file_fd = open(file_name, O_RDWR, S_IRWXU);
	if(file_fd == -1){
		perror("open_decrypt");
		exit(EXIT_FAILURE);
	}

	parse_magic(file_fd);

	if(pipe(pipe_crypt_unarchive) == -1){
		perror("pipe_crypt_unarchive");
		exit(EXIT_FAILURE);
	}

	pid_crypt = fork();
	if(pid_crypt == -1){
		perror("fork_crypt");
		exit(EXIT_FAILURE);
	}

	if (pid_crypt == 0) {
		close(pipe_crypt_unarchive[0]);
		dup2(pipe_crypt_unarchive[1], 1);
		close(pipe_crypt_unarchive[1]);

		lseek(file_fd, strlen("P2CRYPTAR"), SEEK_SET);//We want to skip the bytes
		// that contain the magic number.

		dup2(file_fd, 0);
		close(file_fd);

		execlp("./p2crypt", "p2crypt", encrypt_key, NULL);
		/*if we manage to reach line 258 it means that execlp has failed.
		so we must print an error message*/
		perror("execlp_p2crypt");
		exit(EXIT_FAILURE);
	}
	close(file_fd);
	close(pipe_crypt_unarchive[1]);

	pid_unarchive = fork();
	if(pid_unarchive == -1){
		perror("fork_unarchive");
		exit(EXIT_FAILURE);
	}

	if(pid_unarchive == 0){
		dup2(pipe_crypt_unarchive[0],0);
		close(pipe_crypt_unarchive[0]);

		execlp("./p2unarchive", "unarchive", directory_name, NULL);
		/*if we manage to reach line 277 it means that execlp has failed.
		so we must print an error message*/
		perror("execlp_unarchive");
		exit(EXIT_FAILURE);
	}
	close(pipe_crypt_unarchive[0]);

	for (i=0; i<2; i++){
		waitpid(-1, &status, 0);
		if(WIFEXITED(status)){
			if (WEXITSTATUS(status) == 1){
				//child returned 1 (error)
				exit(EXIT_FAILURE);
			}
		}
	}
}
