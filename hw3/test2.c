/* To stadio 1 tis ergasias alla sleep 
 * Olympia Axelou 2161 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

#define PERIOD_TIME 5

// 0: leitourgei kanonika & 1: restart ton timer se 0
volatile sig_atomic_t restart_timer = 0;

static void handler_sigusr1(int sig){
  
  restart_timer = 1;
}

int main(int argc,char *argv[]){
  
  int i, max_time;
  struct sigaction sigusr1_act={{0}};
  sigset_t set_usr1;
  
  if (argc != 5){
    printf ("Wrong number of arguments\n");
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
  
  for(i=1; i<=max_time; i++){
    sleep(PERIOD_TIME);
    
    if(restart_timer == 1){
      i=1;
      restart_timer = 0;
    }
    
    printf("%d : %d/%d\n", (int)getpid(), i, max_time);
    
    if( (argv[4][0] == '1') && (i == max_time/2)){
      sigprocmask(SIG_UNBLOCK, &set_usr1, NULL);
    }
  }
  
  return 0;
}
    