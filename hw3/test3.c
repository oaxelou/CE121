/* Ena test file  
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define PERIOD_TIME 5

int main(int argc,char *argv[]){
	
	int i, max_time;
	
	if(argc != 2){
		printf("Wrong number of arguments for test3.c!\n");
		return 1;
	}
	
	max_time = atoi(argv[1]);
	if(max_time == 0){
		printf("The second argument is wrong!\n");
		return 1;
	}
	
	for(i=1; i<=max_time; i++){
		sleep(PERIOD_TIME);
		printf("%d : %d/%d\n", (int)getpid(), i, max_time);
	}
	return 0;
}