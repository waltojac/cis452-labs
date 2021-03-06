/**
 * Authors: Ben LaFeldt & Jake Walton
 * CIS 452
 * Purpose: Simple Shell program for Lab 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mcheck.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>



#define ARRSIZE 64

int array_push(char*** arr, int size, char* c);
int free_arr(char*** arr, int size);

int main (){

	char input[ARRSIZE];
	char* cmd_args[ARRSIZE];
	pid_t pid;
	int status;
	long lastusec = 0;
	long lastsec = 0;
	long lastnivcsw = 0;

	while(1) {
		struct rusage *resourceUsage = (struct rusage*)malloc(sizeof(struct rusage));

		//User Prompt
		printf("Enter command: ");

		//Get User Input
		fgets(input, 64, stdin);

		//Parse input
		char* token = strtok(input, " \n");

		// Escape if 'quit'
		if(!strcmp(token, "quit")) {
			break;	
		}

		int count = 0;

		//Clear cmd_args
		int i;
		for(i = 0; i < ARRSIZE; i++) {
			cmd_args[i] = (char*)calloc(ARRSIZE, sizeof(char)*ARRSIZE);
		}

		//If the current token exits, push onto array and get next one
		while(token != NULL) {
			strcpy(cmd_args[count], token);
			count = count + 1;
			token = strtok(NULL, " \n"); 
		}

		//Null terminate vector
		cmd_args[count] = NULL;;

		//Spawn child process
		pid = fork();
		if(pid < 0)
		{
			fprintf(stderr, "An error occurred while forking");
			return -1;
		} else if (pid) {
			//getrusage(pid, resourceUsage);
			//long time = 0;
			//getrusage(pid, resourceUsage);
			//time = resourceUsage->ru_utime.tv_usec;
			waitpid(pid, &status, 0);
			getrusage(RUSAGE_CHILDREN, resourceUsage);
			//time = resourceUsage->ru_utime.tv_usec - time;
			//printf("%ul\n", resourceUsage->ru_utime.tv_usec);
			printf("time: %lu Seconds, %lu Microseconds\n", resourceUsage->ru_utime.tv_sec - lastsec, resourceUsage->ru_utime.tv_usec - lastusec);
			printf("Involuntary Context Switches: %lu\n", resourceUsage->ru_nivcsw - lastnivcsw);
			lastusec = resourceUsage->ru_utime.tv_usec;
			lastsec = resourceUsage->ru_utime.tv_sec;
			lastnivcsw = resourceUsage->ru_nivcsw;
		} else {
			if (execvp(cmd_args[0], &(cmd_args[0])) < 0) {
				exit(errno);
			}
		}
	}

	//Clear cmd_args
	int i;
	for(i = 0; i < ARRSIZE; i++) {
		free(cmd_args[i]);
	}

	return 0;
}
