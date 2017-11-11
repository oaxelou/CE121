/*   Olympia Axelou 2161
 * May 19th, 2016
 * Communication between two users. (message queues)
 */

#include "synchron_functions.h"

#define ANSI_COLOR_RED   "\x1b[1;31m"
#define ANSI_COLOR_RESET "\x1b[0m"

volatile sig_atomic_t sigint_check = 0;

static void handler_sigint(int sig){
	sigint_check = 1;
}

void change_signal_handler(int signum, void *signal_handler){
	
	struct sigaction act = {{0}};
	
	act.sa_handler = signal_handler;
	act.sa_flags = SA_RESTART;
	
	if(sigaction(signum, &act, NULL) == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
}

void send_signal(pid_t pid, int sig){
	
	if(kill(pid, sig) == -1){
		perror("kill");
		exit(EXIT_FAILURE);
	}
}

int main(int argc,char *argv[]){
	
	int shmid, i, msqid, flags;
	key_t key;
	pid_t pid;
	char username[MAX_USERNAME + 1], other_username[MAX_USERNAME + 1];
	char user_id, *write_flag, *read_flag, *buffer, *point_segm;
	char check_if_last;
	int write_rcv, write_send, read_rcv, read_send;
	
	if(argc != 3){
		printf("Wrong number of arguments!\n");
		return 1;
	}
	
	strcpy(username, argv[1]);
	strcpy(other_username, argv[2]);
	
	change_signal_handler(SIGINT, handler_sigint);
	
	key = ftok("partA.c", 'o');
	if(key == -1){
		perror("ftok");
		return 1;
	}
	
	shmid = shmget(key, SHM_SIZE * sizeof(char), IPC_CREAT | IPC_EXCL | S_IRWXU);
	if(shmid == -1){
		if(errno != EEXIST){
			perror("shmget");
			return 1;
		}
		else{
			shmid = shmget(key, 0, 0);
			if(shmid == -1){
				perror("shmget");
				return 1;
			}
			user_id = SECOND_USR;
			
			msqid = my_msgget(key, 0);
		}
	}else{
		user_id = FIRST_USR;
		
		//dimiourgia msgqueue
		msqid = my_msgget(key, IPC_CREAT | IPC_EXCL | S_IRWXU);
		
	}
	
	point_segm = shmat(shmid, NULL, 0);
	if(point_segm == (void *) -1){
		perror("shmat");
		return 1;
	}
	
	// ************************ RUTHMISH GIA KATHE USER **********************
	
	if(user_id == SECOND_USR){
		//tsekarei an antistoixoun ta zeugaria me ta onomata sto shared mem
		if((strcmp(&point_segm[1], username) ||
			strcmp(&point_segm[SECOND_HALF], other_username)) &&
			(strcmp(&point_segm[1], other_username) ||
			strcmp(&point_segm[SECOND_HALF], username))){
			
			printf("Names of users are wrong!\n");
			printf("Try again!\n");
			exit(EXIT_FAILURE);
		}
		
		if(strcmp(&point_segm[1], username) == 0){
			//autos einai o FIRST_USR (vgike kai ksanampike)
			
			write_flag = &point_segm[1 + MAX_USERNAME];
			read_flag = &point_segm[SECOND_HALF + MAX_USERNAME];
			
			set_commun_types(&write_send, &read_send, &write_rcv, &read_rcv, FIRST_USR);
		}
		else{
			//autos einai o SECOND_USR (eite mpainei gia 1h fora eite oxi)
			
			write_flag = &point_segm[SECOND_HALF + MAX_USERNAME];
			read_flag = &point_segm[1 + MAX_USERNAME];
			
			set_commun_types(&write_send, &read_send, &write_rcv, &read_rcv, SECOND_USR);
		}
		*point_segm = '2';
		
		send_msg(msqid, READY);
	}
	if(user_id == FIRST_USR){ 
		write_flag = point_segm + 1;
		for(i=0; i<=strlen(username); i++){
			write_flag[i] = username[i];
		}
		
		write_flag = &point_segm[SECOND_HALF];
		for(i=0; i<=strlen(other_username); i++){
			write_flag[i] = other_username[i];
		}
		
		write_flag = &point_segm[1 + MAX_USERNAME];
		read_flag = &point_segm[SECOND_HALF + MAX_USERNAME];
		
		*point_segm = '1';
		
		//set the msgqueue flags
		set_commun_types(&write_send, &read_send, &write_rcv, &read_rcv, FIRST_USR);
		
		rcv_msg(msqid, READY); //mplokarei mexri na etoimastei o deuteros xristis
	}
	
	// **************************      READ SECTION      ******************************
	
	pid = fork();
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0){
		
		change_signal_handler(SIGINT, SIG_IGN);
		
		buffer = read_flag;
		while(1){
			rcv_msg(msqid, read_rcv);
			
			printf(ANSI_COLOR_RED);
			
			printf(" %s: ", &read_flag[-MAX_USERNAME]);
			for(i=0; buffer[i] != '\0'; i++){
				printf("%c", buffer[i]);
			}
			printf(ANSI_COLOR_RESET"\n");
			
			if(strcmp(buffer, "quit")){
				send_msg(msqid, read_send);
			}
		}
	}
	
	// **********************     WRITE SECTION     ************************
	flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	
	buffer = write_flag;
	
	while(1){
		i = 0;
		buffer[0] = '\0';
		do{
			do{
				scanf("%c", &buffer[i]);
				
				if(sigint_check){
					
					send_signal(pid, SIGTERM);
					waitpid(pid, NULL, 0);
					
					(*point_segm)--;
					check_if_last = *point_segm;
					
					shmdt(point_segm);
					
					if(check_if_last == '0'){
						if(shmctl(shmid, IPC_RMID, NULL) == -1){
							perror("shmctl");
						}
						destroy_msgqueue(msqid);
						
						printf("Goodbye!\n");
					}
					exit(EXIT_SUCCESS);
				}
			}while(buffer[i] != '\n' && buffer[i] < ' ');
			
			if(buffer[0] == '\n'){
				continue;
			}
			if(buffer[i] == '\n'){
				break;
			}
			i++;
		}while(i<MAX_MSG_SIZE);
		
		buffer[i] = '\0';
		
		send_msg(msqid, write_send); //ksemplokarei tin read diergasia tou allou xristi
		
		if(!strcmp(buffer, "quit")){
			break;
		}
		rcv_msg(msqid, write_rcv); //mplokarei mexri na diavasei o allos to minuma pou esteile
	}
	
	//******************* TERMATISMOS ME quit  *********************
	
	(*point_segm)--;
	send_signal(pid, SIGTERM);
	waitpid(pid, NULL, 0);
	
	check_if_last = *point_segm;
	
	shmdt(point_segm);
	
	if(check_if_last == '0'){
		if(shmctl(shmid, IPC_RMID, NULL) == -1){
			perror("shmctl");
		}
		destroy_msgqueue(msqid);
		
		printf("Goodbye!\n");
	}
	return 0;
}