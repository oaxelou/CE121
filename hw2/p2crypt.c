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

#define END_OF_FILE 13
#define FAILURE -1
#define DESIRED_ARGS 2

void argc_check(int argc){
	if(argc != DESIRED_ARGS ){
	  fprintf (stderr,"Not the right number of arguments. Terminating...\n");
	  exit(EXIT_FAILURE);
	}
}

int my_write(int fd, const void *buffer,int count,int line){
    int bytes_written;

    bytes_written = (int) write(fd, buffer, (size_t) count);
    if(bytes_written == FAILURE){
        fprintf(stderr,"write error / line %d \n",line);
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
        fprintf(stderr,"read error on line %d\n", line);
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    else{
        if(!bytes_read){
            return(END_OF_FILE);
        }
        else{
			return(bytes_read);
		}
    }
}

int main (int argc,char *argv[]){
    int i=0,read_check;
    char encoded,to_be_encrypted;

    argc_check(argc);

    while(1){
        read_check = my_read(0,&to_be_encrypted,sizeof(char),__LINE__);
        if(read_check == END_OF_FILE){
            break;
        }
		//To ensure that we start from the first "letter" of our key we
		//restore our counter to 0.
        if( i >= strlen(argv[1]) ){
            i=0;
        }

        encoded = to_be_encrypted ^ argv[1][i]; //XOR to "encrypt" our data,
												//char by char..
        printf("%c",encoded);
        i++;//we increase our counter's value because we want to "find" the next
		//"letter" of our key.
    }

    return(0);
}
