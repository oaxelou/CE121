/* Olympia Axelou 2161 Vassilis Samaras 2144 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_PATH_LENGTH 255
#define FORMAT_STRING_LENGTH 13
#define DESIRED_ARGS 2

void argc_check(int argc){

	if(argc != DESIRED_ARGS ){
	  fprintf (stderr,"Not the right number of arguments. Terminating...\n");
	  exit(EXIT_FAILURE);
	}
}

int main (int argc,char *argv[]){

	DIR *d;
	struct dirent *dir;
	struct stat buf;
	char path[MAX_PATH_LENGTH + 1],*getcwd_check;
	char full_directory_path[MAX_PATH_LENGTH + 1];
	int chdir_check;

	argc_check(argc);

	strcpy(path,argv[1]);

	d = opendir(path);
	if (d == NULL){
		perror ("opendir");
		exit (EXIT_FAILURE);
	}

	chdir_check = chdir(path);
	if ( chdir_check != 0 ){
		perror("chdir");
		exit(EXIT_FAILURE);
	}

	getcwd_check = getcwd(full_directory_path, sizeof(full_directory_path));
	if(getcwd_check == NULL){
		perror("getcwd");
		exit(EXIT_FAILURE);
	}

	while ((dir = readdir(d)) != NULL){

		if ( dir->d_name[0] == '.' ){
			continue;
		}

		if (stat(dir->d_name, &buf) == -1) {
			perror("stat");
			exit(EXIT_FAILURE);
		}

		if (S_ISREG(buf.st_mode)) {
			printf ("%s/", full_directory_path);
			printf ("%s\n", dir->d_name);
		}
	}

	return 0;
}
