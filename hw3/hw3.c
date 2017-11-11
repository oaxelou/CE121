/* OLYMPIA AXELOU 2161
 * 
 * Shell simulation. */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include "lists_functions.h"

#define PERIOD_TIME 20

processT *head;
volatile sig_atomic_t check_status = 0;
volatile sig_atomic_t check_change_running_process = 0;

int get_arguments(char ***argv);
pid_t get_pid();
void set_alarm();
void check_process_status();
void send_signal(pid_t pid, int signum);
void change_signal_handler(int signum, void *signal_handler, int sa_flags);
void change_running_process();
int check_users_answer(char *command);

void exec_command();
void sig_command();
void term_command();
void quit_command();

//edw tha mpei kai otan dimiourgithei mia diergasia
//oxi mono otan termatisei h mplokaristei
static void handler_sigchld(int sig){
	check_status = 1;
}

static void handler_sigalrm(int sig){
	
	if(write(STDOUT_FILENO, "BEEP!\n", 6) == -1){
		perror("write");
		exit(EXIT_FAILURE);
	}
	
	check_change_running_process = 1;
}

void exec_command(){
	
	char **argv;
	pid_t pid;
	processT *new;
	
	if(get_arguments(&argv)){
		return;
	}
	
	pid = fork();
	
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0){
		execvp(argv[0], argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	
	//dimiourgia neou komvou
	new = (processT *) malloc(sizeof(processT));
	if(new == NULL){
		printf ("Error allocating memory!\n");
		destroy_processes(head, head);
		
		exit(EXIT_FAILURE);
	}
	
	new->progname = (char *) malloc(strlen(argv[0]));
	if(new->progname == NULL){
		printf("Error allocating memory\n");
		destroy_processes(head, head);
		
		exit(EXIT_FAILURE);
	}
	strcpy(new->progname, argv[0]);
	new->args = argv;
	new->pid = pid;
	
	if(check_if_empty_list(head)){
		new->status = RUNNING_PROCESS;
		add_process(head, new);
		set_alarm();
	}
	else{
		new->status = BLOCKED_PROCESS;
		add_process(head, new);
		
		send_signal(pid, SIGSTOP);
	}
}

void quit_command(){
	
	pid_t waitpid_return;
	processT *curr;
	
	change_signal_handler(SIGALRM, SIG_IGN, 0);
	
	for(curr=head->next; curr!=head; curr=curr->next){
		send_signal(curr->pid, SIGKILL);
		
		waitpid_return = waitpid(curr->pid, NULL, 0);
		if(waitpid_return == -1){
			perror("waitpid_return");
		}
		else{
// 			printf("\tChild that returned : %d\n", waitpid_return);
			
		}
	}
	
	if(!check_if_empty_list(head)){
		printf("Processes killed:\n");
		destroy_processes(head,head->next);
	}
}

void term_command(){
	pid_t pid;
	
	pid = get_pid();
	if((int)pid == 0){
		return;
	}
		
	send_signal(pid, SIGTERM);

}

void sig_command(){
	
	pid_t pid;
	
	pid = get_pid();
	if((int)pid == 0){
		return;
	}
		
	send_signal(pid, SIGUSR1);
}

int main(int argc,char *argv[]){
	
	char command[5];
	int i;
	int flags;
	
	//ruthmisi wste na min mplokarei otan perimenei na diavasei apo to pliktrologio
	flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	
	//diaxeirisi simatwn: SIGUSR1, SIGCHLD, SIGALRM
	change_signal_handler(SIGUSR1, SIG_IGN, 0); //agnoise  to SIGUSR1
	change_signal_handler(SIGCHLD,handler_sigchld,0); 
	change_signal_handler(SIGALRM, SIG_IGN, 0); //arxika adeia lista (den xreiazetai to ksupnitiri)
	
	init_list(&head);
	
	do{
		
		printf(">$ ");
		
		//diavazei tin entoli tou xristi
		i=0;
		do{
			//non-blocking to STDIN_FILENO opote trexei sunexws kai kanei elegxo
			//me tis check_status kai check_change_running_process
			do{
				command[i] = getchar();
				
				if(check_status){
					check_process_status();
					check_status = 0;
				}
				
				if(check_change_running_process){
					change_running_process();
					check_change_running_process = 0;
				}
			}while((command[i] != '\n') && (command[i] < ' ')); //diladi oso den einai ektupwsimos xaraktiras
			
			if(command[i] == '\n' || command[i] == ' '){
				command[i]='\0';
				break;
			}
			i++;
		}while(1); //edw exei oloklirwthei to string me tin epilogi tou xristi
		
		if(check_users_answer(command)){
			break;
		}
	}while(1);
	
	return 0;
}

//return 1 for quit else 0
int check_users_answer(char *command){
	
	if(strcmp(command,"quit") == 0){
		quit_command();
		return 1;
	}
	else if(strcmp(command, "info") == 0){
		print_processes(head);
	}
	else if(strcmp(command, "exec") == 0){
		exec_command();
	}
	else if(strcmp(command, "term") == 0){
		term_command();
	}
	else if(strcmp(command, "sig") == 0){
		sig_command();
	}
	else if(command[0] == '\0'){
		//do nothing
	}
	else{
		printf("No such command. Try again\n");
	}
	return 0;
}

void set_alarm(){
	
	struct itimerval alarm_period = { {0} };
	struct sigaction sigalrm_act = { {0} }; 
	
	//ruthmisi tis periodou
	alarm_period.it_interval.tv_sec = PERIOD_TIME;
	alarm_period.it_interval.tv_usec = 0;
	
	alarm_period.it_value.tv_sec = 1;
	alarm_period.it_value.tv_usec = 0;
	
	sigalrm_act.sa_handler = handler_sigalrm;
	sigalrm_act.sa_flags = SA_RESTART;   //flag gia ta system calls
	sigaction(SIGALRM, &sigalrm_act, NULL);
	
	//ksekinaei to metrima
	setitimer(ITIMER_REAL, &alarm_period, NULL);
	
}

void change_running_process(){
	
	pid_t running_process, next_to_run_process;
	
	running_process = find_running_process(head);
	next_to_run_process = find_next_running_proc(head);
	
	if(running_process == (pid_t)0){
		printf("\t\tno running process!\n"); //den prepei na ektupwthei auto
		return;
	}
	
	send_signal(running_process, SIGSTOP);
	send_signal(next_to_run_process, SIGCONT);
}

int get_arguments(char ***argv){
	
	char **args, byte_read;
	int i, arg_num=0;
	
	args = (char **)malloc(1 * sizeof(char*));
	if(args == NULL){
		printf("Error allocating memory\n");
		destroy_processes(head, head);
		
		exit(EXIT_FAILURE);;
	}
	args[0] = NULL;
	
	while(1){
		
		do{
			byte_read=getchar();
			
			if((byte_read != '\n') && (byte_read < ' ')){
				printf("No arguments given!\n");
				return 1;
			}
		}while(byte_read == ' ');
		
		if(byte_read == '\n'){
		printf("No arguments given !\n");
		return 1;
		}
		
		//vgainei apo to loop mono an diavasei to \n
		if(byte_read == '\n'){
			break;
		}
		
		//se auto to simeio exei mono xaraktira ASCII
		
		/*to argnum deixnei se poio args[arg_num] anaferomaste, me arg_num>=0 theloume panta 
		 * na exoume enan termatiko komvo (NULL) an exei dothei ena toulaxiston 
		 * orisma to arg_num tha einai toulaxiston 2*/
		args = (char **)realloc(args, (arg_num + 2) * sizeof(char*));
		if(args == NULL){
			printf("Error allocating memory\n");
			exit(EXIT_FAILURE);
		}
		
		args[arg_num] = (char *)malloc( MAX_ARG_SIZE * sizeof(char));
		if (args[arg_num] == NULL){
			printf("Error allocating memory\n");
			destroy_processes(head, head);
			
			exit(EXIT_FAILURE);
		}
		i=0;
		do{
			args[arg_num][i]=byte_read;
			byte_read = getchar();
			i++;
		}while((byte_read != ' ') && (byte_read != '\n'));
		args[arg_num][i]='\0';
		arg_num++;
		if(byte_read == '\n'){
			break;
		}
	}
	args[arg_num] = NULL;
	
	*argv = args;
	
	return 0;
}

void send_signal(pid_t pid, int signum){
	
	if(kill(pid, signum) == -1){
		perror("kill");
	}
}

void change_signal_handler(int signum, void *signal_handler, int sa_flags){
	
	struct sigaction act={{0}};
	
	act.sa_handler = signal_handler;
	act.sa_flags = sa_flags;
	sigaction(signum, &act, NULL);
}

pid_t get_pid(){
	
	char b, pid_string[MAX_ARG_SIZE];
	int i;
	pid_t pid;
	
	do{
		b = getchar();
		if((b != '\n') && (b < ' ')){
			printf("No pid given!\n");
			return ((pid_t)0);
		}
	}while(b == ' ');
	
	if(b == '\n'){
		printf("No pid given !\n");
		return ((pid_t)0);
	}
	i=0;
	while((b !=' ') && (b != '\n')){
		pid_string[i] = b;
		b=getchar();
		i++;
	}
	pid_string[i] = '\0';
	
	pid = (pid_t) atoi(pid_string);
	if((int)pid == 0){
		printf("Wrong argument. It must be an integer!\n");
		return ((pid_t)0);
	}
	return pid;
}

void check_process_status(){
	
	pid_t pid, next_to_run_process;
	processT *curr, *prev;
	int status;
	
	for(curr=head->next, prev=head; curr!=head; ){
		
		prev=curr;
		curr=curr->next;
		
		pid=waitpid(prev->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
		
		if(pid == -1){
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0){
			//printf("no child exited/stopped/terminated\n");
		}
		else{
			if(WIFEXITED(status)){ 
				//elegxos an uparxoun ki alles diergasies pou ekkremoun
				if(!check_if_one_node_list(head)){
					next_to_run_process = find_next_running_proc(head);
					send_signal(next_to_run_process, SIGCONT);
					remove_process(head, prev->pid);
					return;
				}
				
				remove_process(head, prev->pid);
				change_signal_handler(SIGALRM, SIG_IGN, 0);
			}
			else if(WIFSIGNALED(status)){
				//elegxos an uparxoun ki alles diergasies pou ekkremoun
				if(!check_if_one_node_list(head)){
					next_to_run_process = find_next_running_proc(head);
					send_signal(next_to_run_process, SIGCONT);
					remove_process(head, prev->pid);
					return;
				}
				
				remove_process(head, prev->pid);
				change_signal_handler(SIGALRM, SIG_IGN, 0);
			}
			else if(WIFSTOPPED(status)){
				change_running_status(head, prev->pid, BLOCKED_PROCESS);
			}
			else if(WIFCONTINUED(status)){
				change_running_status(head, prev->pid, RUNNING_PROCESS);
			}
		}
	}
	
}