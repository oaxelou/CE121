/* Axelou Olympia 2161 */

#include "synchron_functions.h"

// *************** WITH MESSAGE QUEUES ******************

int my_msgget(key_t key, int flags){
	
	int msqid;
	
	msqid = msgget(key, flags);
	if(msqid == -1){
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	return msqid;
}

/* H sunartisi auti orizei ta types gia an h epikoinwnia einai message queues
 *   kai ton arithmo tou simatoforou an einai semaphores
 * Orizei ta antistoixa noumera an einai o FIRST_USR kai an einai o SECOND_USR */
void set_commun_types(int *write_send, int *read_send, int *write_rcv, int *read_rcv, char num_of_usr){
	
	if(num_of_usr == SECOND_USR){
		*write_send = SECOND_WROTE;
		*read_send = SECOND_READ;
		*write_rcv = FIRST_READ;
		*read_rcv = FIRST_WROTE;
	}
	else{
		*write_send = FIRST_WROTE;
		*read_send = FIRST_READ;
		*write_rcv = SECOND_READ;
		*read_rcv = SECOND_WROTE;
	}
}

void send_msg(int msqid, long type){
	
	struct msg m;
	
	m.type = type;
	strcpy(m.data, "\0");
	
	if(msgsnd(msqid, &m, 1, 0) == -1){
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
}

void rcv_msg(int msqid, long type){
	
	struct msg m;
	
	if(msgrcv(msqid, &m, 1, type, 0) == -1){
		if(errno != EINTR){ //gia tin periptwsi pou erthei SIGINT enw einai mplokarismenos
			perror("msgrcv");
			exit(EXIT_FAILURE);
		}
	}
}

void destroy_msgqueue(int msqid){
	
	if(msgctl(msqid, IPC_RMID, NULL) == -1){
		perror("msgctl");
		exit(EXIT_FAILURE);
	}
}


// ****************  WITH SEMAPHORES ***********************

int my_semget(key_t key, int flags){
	
	int semid;
	
	semid = semget(key, 5, flags);
	if(semid == -1){
		perror("semget");
		exit(EXIT_FAILURE);
	}
	return semid;
}

void destroy_sem(int semid){
	
	if(semctl(semid, 0, IPC_RMID) == -1){  //0 is ignored!
		perror("semctl");
		exit(EXIT_FAILURE);
	}
}

void init_sem(int semid){
	
	if(semctl(semid, SEM_READY, SETVAL, 0) == -1){
		perror("semctl (SEM_READY)");
		exit(EXIT_FAILURE);
	}
	
	if(semctl(semid, FIRST_WROTE, SETVAL, 0) == -1){
		perror("semctl (FIRST_WROTE)");
		exit(EXIT_FAILURE);
	}
	
	if(semctl(semid, SECOND_WROTE, SETVAL, 0) == -1){
		perror("semctl (SECOND_WROTE)");
		exit(EXIT_FAILURE);
	}
	
	if(semctl(semid, FIRST_READ, SETVAL, 0) == -1){
		perror("semctl (FIRST_READ)");
		exit(EXIT_FAILURE);
	}
	
	if(semctl(semid, SECOND_READ, SETVAL, 0) == -1){
		perror("semctl (SECOND_READ)");
		exit(EXIT_FAILURE);
	}
}

void my_semop(int semid, int type, int op){
	
	struct sembuf oper;
	
	oper.sem_num = type;
	oper.sem_op = op;
	oper.sem_flg = 0;
	
	if(semop(semid, &oper, 1) == -1){
		if(errno != EINTR){ //gia tin periptwsi pou erthei SIGINT enw einai mplokarismenos
			perror("semop");
			exit(EXIT_FAILURE);
		}
	}
}
