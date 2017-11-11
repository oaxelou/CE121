/* Axelou Olympia 2161 */

#ifndef __SYNCHRON_FUNCTIONS__
#define __SYNCHRON_FUNCTIONS__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define FIRST_USR '1'
#define SECOND_USR '2'

//defines gia to shared memory
#define MAX_USERNAME 20
#define MAX_FORMAT 20
#define MAX_MSG_SIZE 64 //xwris to backslash-0
#define SHM_SIZE (1 + 2*MAX_MSG_SIZE + 2*MAX_USERNAME)
#define SECOND_HALF (1 + MAX_MSG_SIZE  + MAX_USERNAME)

//defines gia tin epikoinwnia

#define SEM_READY 0
#define FIRST_READ 1
#define SECOND_READ 2
#define FIRST_WROTE 3
#define SECOND_WROTE 4
#define READY 5

/*H perioxi tis koinis mnimis exei tin morfi:
 * <flag gia diagrafi shared memory + epikoinwnias (1byte)> <name1(MAX_USERNAME bytes)> < MAX_MSG_SIZE(64bytes)>
 * <name2(MAX_USERNAME bytes)> < MAX_MSG_SIZE(64bytes)>
 * 
 * Sto name1 einai autos pou dimiourgise to shared memory (akoma kai an ksanampei) */

// *****************   MESSAGE QUEUES   *********************

struct msg{
	long type;
	char data[1];
};

int my_msgget(key_t key, int flags);
void destroy_msgqueue(int msqid);
void send_msg(int msqid, long type);
void rcv_msg(int msqid, long type);
void set_commun_types(int *write_send,int *read_send,int *write_rcv,int *read_rcv, char num_of_usr);

// *****************    SEMAPHORES    ***********************

int my_semget(key_t key, int flags);
void destroy_sem(int semid);
void init_sem(int semid);
void my_semop(int semid, int type, int op);

#endif