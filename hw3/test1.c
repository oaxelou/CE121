/* To stadio 1 tou hw3 (me simata)
 * Olympia Axelou 2161 */
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define PERIOD_TIME 5

// 0: leitourgei kanonika & 1: restart ton timer se 0
volatile sig_atomic_t restart_timer = 0;
volatile sig_atomic_t print_timer = 0;

static void handler_sigusr1(int sig){
	restart_timer = 1;
}

static void handler_sigalrm(int sig){
	print_timer = 1;
}

int main(int argc,char *argv[]){
	
	int i, max_time;
	struct itimerval alarm_period = { {0} };
	struct sigaction sigusr1_act = { {0} }, sigalrm_act = { {0} }; 
	sigset_t set_usr1;
	
	if(argc != 5){
		printf ("Wrong number of arguments\n");
		return 1;
	}
	
	if(argv[1][1] != 'm'){
		printf("The first argument is wrong\n");
		return 1;
	}
	
	if(argv[3][1] != 'b'){
		printf("The third argument is wrong\n");
		return 1;
	}
	
	max_time = atoi(argv[2]);
	if(max_time == 0){
		printf ("No integer given for max_time\n");
		return 1;
	}
	
	if ((argv[4][0] != '0') && (argv[4][0] != '1')){
		printf ("The 4th argument is wrong\n");
		return 1;
	}
	
	//allazoume to pws metaxeirizetai to programma to sima SIGUSR1
	sigusr1_act.sa_handler = handler_sigusr1;
	sigaction(SIGUSR1, &sigusr1_act, NULL);
	
	//ftiaxnei ena set apo signals pou periexei mono to SIGUSR1
	sigemptyset(&set_usr1);
	sigaddset(&set_usr1, SIGUSR1);
	
	if (argv[4][0] == '1'){
		sigprocmask(SIG_BLOCK, &set_usr1, NULL); //mplokare to SIGUSR1
	}
	
	//thetoume tin periodo
	alarm_period.it_interval.tv_sec = PERIOD_TIME;
	alarm_period.it_interval.tv_usec = 0;
	
	//thetoume oti tha ksekinisoume to metrima apo ti stigmi pou tha kalesoume tin setitimer
	alarm_period.it_value.tv_sec = PERIOD_TIME;
	alarm_period.it_value.tv_usec = 0;
	
	//allazoume to pws metaxeirizetai to programma to sima SIGALRM
	sigalrm_act.sa_handler = handler_sigalrm;
	sigaction(SIGALRM, &sigalrm_act, NULL);
	
	//ksekinaei to metrima
	setitimer(ITIMER_REAL, &alarm_period, NULL);
	
	i=0;
	while(i < max_time){
		
		if(print_timer == 1){
			i++;
			printf("%d : %d/%d\n", (int)getpid(), i, max_time);
			print_timer = 0;
		}
		
		if (restart_timer == 1){
			i=0;
			restart_timer = 0; 
		}
		
		if( (argv[4][0] == '1') && (i == max_time/2)){
			sigprocmask(SIG_UNBLOCK, &set_usr1, NULL);
		}
		
	}
	return 0;
}