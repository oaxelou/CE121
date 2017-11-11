/*                 Olympia Axelou 2161 Vassilis Samaras 2144 
 * 
 * To programma auto anaparista mia vasi dedomenwn : 
 * (eisagwgi arxeiou, anazitisi,eksagwgi, diagrafi) 
 * 
 * H domi tis vasis einai:
 * <MAGIC_NUMBER>,<onoma1>,<megethos1(se int)>,<arxeio1>, klp
 * */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define BYTES_PARTS 512
#define MAX_FILE_NAME 255
#define FORMAT_STRING_LENGTH 16
#define FAILURE -1
#define END_OF_FILE 13
#define MAGIC_NUMBER "49/7753/37"
#define SIZE_MAGIC_NUMBER strlen(MAGIC_NUMBER)
#define UPPER_CHOICE_LIMIT '5'
#define LOWER_CHOICE_LIMIT '1'
#define EXIT '5'
#define DELETE '4'
#define EXPORT '3'
#define SEARCH '2'
#define IMPORT '1'
#define NOT_FOUND 0
#define FOUND_AT_LEAST_ONCE 1

//sunartiseis pou emperiexoun tous elegxous twn system calls
int my_lseek(int fd, int offset, int whence, int line);
int my_read(int fd, void *buf, int nbytes, int line);
int my_write(int fd, const void *buffer,int count,int line);
int my_open(const char *pathname,int flags,mode_t mode,int line);
int my_open_export(const char *pathname, int line);
int my_database_open(const char *pathname,int flags,mode_t mode,int line);

//voithitikes sunartiseis
void argc_check(int argc);
void print_menu();
char user_choice();
void check_magic_number(int fd, char signature[]);
void no_path(char *text_file_name);
void offset_del_prep(int fd_database, int offset, char *database);
void copy_until_eof(int fd1, int fd2, int offset1, int offset2);
int search_name(int fd_database, char *object);
char search_curr_name(int fd, char *object);
void copy_specific_file(int fd_database,int fd_file,int offset,int size_file);

//kuries sunartiseis (den epistrefoun kati, se periptwsi lathous: EXIT)
void import(char *database);
void search(char *database);
void delete(char *database);
void export(char *database);

int main(int argc, char *argv[]) {
	
	char choice;
	
	argc_check(argc);
	
	do{
		choice=user_choice();
		
		switch (choice) {
			case IMPORT:
				import(argv[1]);
				break;
			
			case SEARCH:
				search(argv[1]);
				break;
			
			case EXPORT:
				export(argv[1]);
				break;
			
			case DELETE:
				delete(argv[1]);
				break;
			
			case EXIT:
				printf("Thank you for your prefference.\n");
				exit(EXIT_SUCCESS);
				break;
		}
	}while(1);
	
	return (0);
}

/* Epistrfei to offset an to system call einai epituxes
 * h termatizei to programma an kati paei strava */
int my_lseek(int fd, int offset, int whence, int line){
	
	int return_value;
	
	return_value = (int) lseek(fd, (off_t) offset, whence);
	
	if( return_value == -1){
		printf("lseek error on line %d: %s\n", line, strerror(errno));
		exit (EXIT_FAILURE);
	}
	return (return_value);
}

/* Epistrfei ta bytes pou diavastikan an to system call einai epituxes
 * h termatizei to programma an kati paei strava */
int my_read(int fd, void *buf, int nbytes, int line){
	
	int bytes_read;
	
	bytes_read = (int) read(fd,buf, (size_t) nbytes);
	
	if(bytes_read == FAILURE){
		printf("read error on line %d: %s\n", line, strerror(errno));
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

/* Epistrfei ta bytes pou graftikan an to system call einai epituxes
 * h termatizei to programma an kati paei strava */
int my_write(int fd, const void *buffer,int count,int line){
	
	int bytes_written;
	
	bytes_written = (int) write(fd, buffer, (size_t) count);
	
	if(bytes_written == FAILURE){
		printf("write error on line %d: %s\n", line, strerror(errno));
		close(fd);
		exit(EXIT_FAILURE);
	}
	else
		return(bytes_written);
}

/*anoigei apla ena text file gia tin import kai epistrefei to fd tou*/
int my_open(const char *pathname,int flags,mode_t mode,int line){
	int fd;
	
	fd = open(pathname,flags,mode);
	
	if(fd == FAILURE ){
		printf("open error on line %d: %s\n", line, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
    return(fd);
}

/*anoigei ena arxeio gia tin export. An uparxei rwtaei ton xristi gia truncate
 *epistrfei to fd */
int my_open_export(const char *pathname, int line){
	
	int fd_file;
	char choice;
	
	fd_file = open(pathname,O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
	
	if(fd_file == FAILURE ){
		
		if(errno == EEXIST){ 
			
			printf("File already exists\n");
			printf("Do you want to truncate it? (y/n) : ");
			scanf(" %c", &choice);
			
			while(choice != 'n' && choice != 'y'){
				printf ("Invalid entry.\n");
				
				printf("File already exists\n");
				printf("Do you want to truncate it? (y/n)");
				scanf(" %c", &choice);
			}
			
			if(choice == 'n')
				exit(EXIT_FAILURE);
			
			//o xristis epelekse truncate
			fd_file = open(pathname, O_RDWR | O_TRUNC , S_IRWXU);
			return (fd_file);
		}
		else{
			printf("open error on line %d: %s\n", line, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	
	return(fd_file);
}

/* Anoigei to arxeio pou exei dothei mesa apo ta orismata kai an uparxei
 * tsekarei an einai arxeio vasis. An den uparxei grafei to anagnwristiko 
 * epistrefei to fd tou database*/
int my_database_open(const char *pathname,int flags,mode_t mode,int line){
	
	int fd;
	
	fd = open(pathname,flags,mode);
	
	if(fd == FAILURE){
		if(errno == EEXIST){ //file already exists
			fd = open(pathname,O_RDWR ,mode);
			if(fd == FAILURE ){
				printf("open error on line %d: %s\n", line, strerror(errno));
				exit(EXIT_FAILURE);
			}
			else{
				check_magic_number(fd,MAGIC_NUMBER);
				return (fd);
			}
		}
		else{
			printf("open error on line %d: %s\n", line, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	// grafei to anagnwristiko (to arxeio database dimiourgithike twra)
	my_write(fd,MAGIC_NUMBER,SIZE_MAGIC_NUMBER * sizeof(char),__LINE__);
	
	printf ("\nDatabase file created.\n\n");
	return(fd);
}

void argc_check(int argc){
	
	if(argc != 2 ){ 
		printf ("Not the right number of arguments. Terminating...\n");
		exit(EXIT_FAILURE);
	}
}

void print_menu(){
	
	printf("\n ================== \n");
	printf(" Menu :\n");
	printf("  1. Import\n");
	printf("  2. Search\n");
	printf("  3. Export\n");
	printf("  4. Delete\n");
	printf("  5. Exit\n");
	printf("\n ================== \n");
}

//tsekarei kai epistrefei tin epilogi tou xristi gia to menu. epistre
char user_choice(){
	
	char choice;
	
	do{
		print_menu();
		
		printf("\nPlease enter your preffered action's number:\n");
		scanf(" %c",&choice);
		if(choice>UPPER_CHOICE_LIMIT || choice<LOWER_CHOICE_LIMIT)
			printf("\nPlease enter a valid number\n");
		
	}while(choice>UPPER_CHOICE_LIMIT || choice<LOWER_CHOICE_LIMIT);
	
	return(choice);
}

//elegxei an h vasi dedomenwn einai egkuri
void check_magic_number(int fd, char signature[]){
	
	char bytes_to_check[ strlen(signature) + 1 ];
	int check_read;
	
	check_read = my_read(fd, bytes_to_check, strlen(signature), __LINE__);
	bytes_to_check[check_read] = '\0';
	
	if( strcmp(bytes_to_check, signature) ){
		printf("Invalid file type for our database. Terminating....\n");
		close(fd);
		exit(EXIT_FAILURE);
	}
	my_lseek(fd, 0, SEEK_END, __LINE__);
}

void no_path(char *text_file_name){
	
	char *last_slash;
	
	last_slash = strrchr(text_file_name,'/');
	if(last_slash != NULL){
		last_slash++;
		strcpy(text_file_name,last_slash);
	}
}

/* Topothetei ta offset stin katallili thesi sto arxeio tis vasis.
 * Kalei tin copy_until_eof kai meta kanei truncate to arxeio tis vasis */
void offset_del_prep(int fd_database, int offset, char *database){
	
	int fd_temp; //voithitiki metavliti gia tin antigrafi twn bytes
	int size_file_to_del, size_database, next_file_offset;
	
	fd_temp = my_open(database, O_RDONLY, S_IRWXU, __LINE__);
	
	//vriskei to megethos tis vasis
	size_database = my_lseek(fd_database, 0, SEEK_END, __LINE__);
	
	//ruthmizei to offset to fd_database na deixnei to 1o byte tou onomatos
	my_lseek(fd_database, offset, SEEK_SET, __LINE__);
	
	//ruthmizei to offset tou fd_temp na deixnei to 1o byte tou int tou megethous
	my_lseek(fd_temp, offset + MAX_FILE_NAME, SEEK_SET, __LINE__);
	
	//diavazei to megethos tou arxeiou gia diagrafi
	my_read(fd_temp, &size_file_to_del, sizeof(int), __LINE__);
	
	//ruthmizei to fd_temp na deixnei sto 1o byte tou epomenou arxeiou
	my_lseek(fd_temp, size_file_to_del, SEEK_CUR, __LINE__);
	
	next_file_offset = offset + MAX_FILE_NAME + size_file_to_del + sizeof(int);
	
	copy_until_eof(fd_database, fd_temp, offset, next_file_offset);
	
	/*meiwnei to database kata (megethos arxeiou + ta bytes pou desmeuoume gia to
	 *megethos arxeiou + ton arithmo twn bytes tou int pou anaparista to megethos*/ 
	truncate(database, size_database - (size_file_to_del + MAX_FILE_NAME + sizeof(int)));
}

/* Orismata: 2 file descriptors kai ta offset tous
 * Ta ruthmisei kai antigrafei ta antistoixa bytes apo to fd2 sto fd1*/
void copy_until_eof(int fd1, int fd2, int offset1, int offset2){
	
	int bytes_read;
	char buffer[BYTES_PARTS];
	
	my_lseek(fd1, offset1, SEEK_SET, __LINE__);
	my_lseek(fd2, offset2, SEEK_SET, __LINE__);
	
	while(1){
		if((bytes_read=my_read(fd2, buffer, BYTES_PARTS, __LINE__)) == END_OF_FILE){
		break;
		}
		my_write(fd1, buffer, bytes_read, __LINE__);
	}
}

/* H antistoixh ths search alla psaxnei gia object olokliro to onoma
 * Epistrfei: an vrethei:     to offset tou onomatos tou arxeiou
 *            an den vrethei: FAILURE */
int search_name(int fd_database, char *object){
	
	char found, byte_to_check;
	int i, curr_name, size_file;
	
	//pernaei to magic_number
	my_lseek(fd_database, SIZE_MAGIC_NUMBER, SEEK_SET, __LINE__);
	
	while(1){
		if(my_read(fd_database, &byte_to_check, 1, __LINE__) == END_OF_FILE){
			found='n';
			break;
		}
		curr_name=my_lseek(fd_database, -1, SEEK_CUR, __LINE__);
		
		found = 'y';
		i = 0;
		
		do{
			my_read(fd_database, &byte_to_check, 1, __LINE__);
			
			//eftase sto telos tou current name enw den exei ftasei sto 
			//telos tou object pou psaxnei, opote sigoura den tautizontai
			if(byte_to_check == '\0'){
				found='n';
				break;
			}
			
			if(byte_to_check != object[i]){
				found='n';
				break;
			}
			i++;
		}while( i < strlen(object) );
		
		//an einai found=='y' tote oi prwtoi strlen(object) chars tou current
		//name tautizontai me to object.gia na tautizontai to onoma kai to 
		//object prepei to epomeno byte na einai to \n
		if(found == 'y'){
			my_read(fd_database, &byte_to_check, 1, __LINE__);
			if(byte_to_check == object[i])
				break;
		}
		//an den to vrike sunexizei sto epomeno onoma
		my_lseek(fd_database, curr_name + MAX_FILE_NAME, SEEK_SET, __LINE__);
		
		my_read(fd_database, &size_file, sizeof(int), __LINE__);
		my_lseek(fd_database, size_file, SEEK_CUR, __LINE__);
	}
	
	if(found == 'y')
		return (curr_name);
	
	return (FAILURE);
}

/* Psaxnei mesa sto onoma an uparxei to object pou edwse o xristis sti search
 * Epistrfei: an vrethei:    'y'
 *            an den vrethei:'n' */
char search_curr_name(int fd, char *object){
	
	int i;
	char byte_to_check, found;
	
	while(1){
		
		if(my_read(fd, &byte_to_check, 1, __LINE__) == END_OF_FILE)
			return 'n';
		
		if(byte_to_check == '\0')
			return 'n';
		
		if (byte_to_check == object[0]){
			found = 'y';
			
			for (i=1; i<strlen(object); i++){
				my_read(fd, &byte_to_check, 1, __LINE__);
				
				if(byte_to_check != object[i]){
					found = 'n';
					break;
				}
				if(byte_to_check == '\0')
					return 'n';
			}
			if (found == 'y')
				return found;
		}
	}
	return ('n');
}

/* antigrafei sugkekrimena bytes apo to database se ena text file sto 
 * opoio epithumoume na kanoume export */
void copy_specific_file(int fd_database, int fd_file, int offset, int size_file){ 
	
	int i, bytes_read, bytes_copied;
	char buffer[BYTES_PARTS];
	
	my_lseek(fd_database, offset, SEEK_SET, __LINE__);
	
	bytes_copied = 0;
	
	for (i=0; i<=size_file/BYTES_PARTS; i++){
		if (size_file - bytes_copied < BYTES_PARTS){ 
			
			//an apomenoun ligotera apo BYTES_PARTS antigrafei osa apemeinan
			bytes_read=my_read(fd_database,buffer,size_file-bytes_copied,__LINE__);
			my_write(fd_file, buffer, bytes_read, __LINE__);
			
			break;
		}
		bytes_read=my_read(fd_database, buffer, BYTES_PARTS, __LINE__);
		bytes_copied += bytes_read;
		
		my_write(fd_file, buffer, bytes_read, __LINE__);
	}
}

void import(char *database){
	
	int fd_database, fd_file, size_file, offset;
	char file_name[MAX_FILE_NAME+1];
	char format[FORMAT_STRING_LENGTH], choice;
	
	fd_database = my_database_open(database, O_RDWR | O_CREAT | O_EXCL,S_IRWXU, __LINE__);
	
	sprintf(format,"%%%ds",MAX_FILE_NAME);
	printf ("Enter file name: ");
	scanf (format,file_name);
	
	offset = search_name(fd_database, file_name); //psaxnei an uparxei hdh to arxeio sti vasi
	
	if(offset != -1){
		printf ("File already exists in database.\n"); 
		printf ("Do you want to replace it? (y/n) ");
		scanf (" %c", &choice);
		
		while(choice != 'y' && choice != 'n'){
			printf ("Invalid entry. (y/n) Try again: ");
			scanf(" %c", &choice);
		}
		if(choice == 'y')
			offset_del_prep(fd_database, offset, database);
		else
			return;
	}
	
	my_lseek(fd_database, 0, SEEK_END, __LINE__);
	
	//anoigei to arxeio gia import kai svinei to pathname an uparxei
	fd_file = my_open(file_name,O_RDONLY ,S_IRWXU, __LINE__);
	no_path(file_name);
	
	// vriskei to megethos tou arxeiou kai gurna to offset stin arxi
	size_file = my_lseek(fd_file, 0, SEEK_END,__LINE__);
	my_lseek(fd_file, 0, SEEK_SET, __LINE__);
	
	//sto onoma apothikeuei kai to \0 gia na dieukolinei tin search kai metakinei to offset gia to size_file
	my_write(fd_database, file_name, strlen(file_name) + 1, __LINE__);
	my_lseek(fd_database,MAX_FILE_NAME - (strlen(file_name) + 1),SEEK_CUR,__LINE__);
	
	my_write(fd_database,&size_file,sizeof(int),__LINE__);
	
	offset = my_lseek(fd_database, 0, SEEK_CUR, __LINE__); //vriskei pou tha grapsei to arxeio
	
	copy_until_eof(fd_database,fd_file,offset, 0);
	
	printf ("\nObject successfully imported.\n");
	
	close(fd_database);
	close(fd_file);
	
	return;
}

void search(char *database){
	
	int fd_database, curr_name, size_file, sum_found;
	char object[MAX_FILE_NAME + 1], format[FORMAT_STRING_LENGTH], byte_to_check;
	char buffer[MAX_FILE_NAME + 1];
	
	fd_database = my_database_open(database, O_RDWR | O_CREAT | O_EXCL,S_IRWXU, __LINE__);
	
	printf ("Enter object to search: ");
	sprintf(format, "%%%ds", MAX_FILE_NAME);
	scanf (format, object);
	
	my_lseek(fd_database, SIZE_MAGIC_NUMBER, SEEK_SET, __LINE__);
	
	if ( strcmp(object, "*") ){ //an h strcmp epistrepsei diaforo tou 0
		
		printf ("\nNames of files including \"%s\" :\n", object);
		
		sum_found=NOT_FOUND;
		while(1){
			//tsekarei an exei ftasei sto teleos tou database
			if( my_read(fd_database,&byte_to_check,1,__LINE__) == END_OF_FILE )
				break;
			
			/*gurnaei to offset stin prohgoumeni thesi epeidi diavase hdh 1 byte 
			 *gia na tsekarei an exei ftasei sto telos tou database */
			curr_name = my_lseek(fd_database, -1, SEEK_CUR, __LINE__); 
			
			//tsekarei to trexon onoma mesw tis search_curr_name
			if (search_curr_name(fd_database, object) == 'y'){
				sum_found = FOUND_AT_LEAST_ONCE;
				
				my_lseek(fd_database, curr_name, SEEK_SET, __LINE__);
				my_read(fd_database, buffer, MAX_FILE_NAME, __LINE__);
				
				printf (" %s\n", buffer);
			}
			else{
				my_lseek(fd_database, curr_name + MAX_FILE_NAME, SEEK_SET, __LINE__);
			}
			
			my_read(fd_database,&size_file, sizeof(int), __LINE__);
			my_lseek(fd_database,size_file, SEEK_CUR, __LINE__);
		}
		if (sum_found == NOT_FOUND)
			printf ("(nothing found)\n");
	}
	else{
		printf ("\nFiles in database:\n");
		
		while(1){
			
			if(my_read(fd_database, &byte_to_check, 1, __LINE__) == END_OF_FILE)
			    break;
			my_lseek(fd_database, -1, SEEK_CUR, __LINE__);
		
		my_read(fd_database, buffer, MAX_FILE_NAME, __LINE__);
		
		printf (" %s\n", buffer);
		
		my_read(fd_database, &size_file, sizeof(int), __LINE__);
		my_lseek(fd_database, size_file, SEEK_CUR, __LINE__);
	  }
	}
	
	return;
}

void export(char *database){
	
	char object[MAX_FILE_NAME + 1], file[MAX_FILE_NAME + 1];
	char format[FORMAT_STRING_LENGTH];
	int offset_object, fd_database, fd_file, size_file;
	
	fd_database = my_database_open(database, O_RDWR | O_CREAT | O_EXCL, S_IRWXU, __LINE__);
	
	sprintf(format, "%%%ds", MAX_FILE_NAME);
	
	printf ("Enter object you want to export: ");
	scanf (format, object);
	
	printf ("Enter file: ");
	scanf (format, file);
	
	offset_object = search_name(fd_database, object);
	
	if(offset_object == FAILURE){
		printf ("There is no entry with name \"%s\"\n", object);
		exit(EXIT_FAILURE);
	}
	fd_file=my_open_export(file, __LINE__);
	
	my_lseek(fd_database, offset_object + MAX_FILE_NAME, SEEK_SET, __LINE__);
	my_read(fd_database, &size_file, sizeof(int), __LINE__);
	
	offset_object = my_lseek(fd_database, 0, SEEK_CUR, __LINE__);
	copy_specific_file(fd_database, fd_file, offset_object, size_file);
	
	printf ("\nObject successfully exported.\n");
	
	close(fd_database);
	close(fd_file);
	
	return;
}

void delete(char *database){
	
	int fd_database, offset;
	char object[MAX_FILE_NAME+1], format[FORMAT_STRING_LENGTH];
	
	fd_database = my_database_open(database, O_RDWR | O_CREAT | O_EXCL, S_IRWXU, __LINE__);
	
	sprintf(format, "%%%ds", MAX_FILE_NAME);
	printf ("Enter object you want to delete: ");
	scanf (format, object);
	
	offset = search_name(fd_database, object);
	if(offset == -1){
		printf ("File does not exist\n");
		return;
	}
	
	offset_del_prep(fd_database, offset, database);
	
	printf ("\nObject successfully deleted.\n");
	
	close(fd_database);
	
	return;
}