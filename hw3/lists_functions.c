/*        OLYMPIA AXELOU 2161 
 * Edw einai ulopoihmenes oles oi sunartiseis pou aforoun tis listes
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "lists_functions.h"

void init_list(processT **head){
	
	(*head) = (processT *)malloc(sizeof(processT));
	if ((*head) == NULL){
		printf("Error allocating memory\n");
		
		//apodesmeuei oli tin idi dunaika desmeumeni mnimi
		destroy_processes(*head, *head);
		
		exit(EXIT_FAILURE);
	}
	(*head)->next = (*head);
	(*head)->prev = (*head);
}

void add_process(processT *head, processT *new){
	
	new->prev = head->prev;
	head->prev->next = new;
	new->next = head;
	head->prev = new;
}

void print_processes(processT *head){
	
	processT *curr;
	int i;
	
	printf ("Processes: \n");
	for(curr=head->next; curr!=head; curr=curr->next){
		
		printf(" pid: %d, ", curr->pid); 
		printf("name: (%s ", curr->progname);
		for(i=1; curr->args[i]!=NULL; i++){ 
			printf(", %s ", curr->args[i]);
		}
		printf(")");
		if(curr->status == RUNNING_PROCESS){
			printf("(R)");
		}
		
		printf("\n");
	}
	printf("\n");
}

void remove_process(processT *head,pid_t pid_to_rem){
	
	processT *curr;
	int i;
	
	for (curr=head->next; curr!=head && curr->pid!=pid_to_rem;curr=curr->next){
	}
	
	if (curr == head){
		printf("No process with pid: %d\n", (int)pid_to_rem);
	}
	else{
		curr->prev->next=curr->next;
		curr->next->prev=curr->prev;
		
		free(curr->progname);
		for(i=0; curr->args[i]!=NULL; i++){
			free(curr->args[i]);
		}
		free(curr->args);
		free(curr);
	}
}

void destroy_processes(processT *head, processT *list_head){
	
	int i;
	
	if(list_head != head){
		
		destroy_processes(head, list_head->next);
		
		printf(" pid: %d\n", (int)list_head->pid);
		
		free(list_head->progname);
		
		for(i=0; list_head->args[i]!=NULL; i++){
			free(list_head->args[i]);
		}
		free(list_head->args);
		free(list_head);
	}
}

pid_t find_running_process(processT *head){
	
	processT *curr;
	
	for(curr=head->next;curr!=head;curr=curr->next){
		if(curr->status == RUNNING_PROCESS){
			return curr->pid;
		}
	}
	return ((pid_t)0);
}

pid_t find_next_running_proc(processT *head){
	
	processT *curr;
	
	for(curr=head->next; curr!=head; curr=curr->next){
		if(curr->status == RUNNING_PROCESS){
			break;
		}
	}
	if (curr == head){
		return ((pid_t)0);
	}
	curr=curr->next;
	if(curr == head){
		curr = head->next;
	}
	return curr->pid;
}

void change_running_status(processT *head, pid_t process, char status){
	
	processT *curr;
	
	for(curr=head->next; curr!=head; curr=curr->next){
		if(curr->pid == process){
			curr->status = status;
		}
	}
}

int check_if_empty_list(processT *head){
	
	if (head->next == head){
		return 1;
	}
	return 0;
}

int check_if_one_node_list(processT *head){
	
	if(head->next->next == head){
		return 1;
	}
	return 0;
}