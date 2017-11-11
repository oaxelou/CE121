#ifndef __LISTS_FUNCTIONS_H__
#define __LISTS_FUNCTIONS_H__

typedef struct process_struct{
	struct process_struct *next;
	struct process_struct *prev;
	pid_t pid;
	char *progname;
	char **args;
	char status;
}processT;

#define RUNNING_PROCESS '1'
#define BLOCKED_PROCESS '0'

#define MAX_PROG_NAME 256
#define MAX_ARG_SIZE 256

void init_list(processT **head);
void add_process(processT *head, processT *new);
void print_processes(processT *head);
void remove_process(processT *head,pid_t pid_to_rem);
void destroy_processes(processT *head,processT *list_head);

pid_t find_running_process(processT *head);
pid_t find_next_running_proc(processT *head);
void change_running_status(processT *head, pid_t process, char status);
int check_if_one_node_list(processT *head); //one:1 not one: 0
int check_if_empty_list(processT *head); //empty:1 not_empty:0

#endif