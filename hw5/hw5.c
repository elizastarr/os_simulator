// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// hw5.c file (main function)
// 
// Homework 5 - OS Simulator
//
// -----------------------------------

#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"

#define MAIN_DEBUG 1


int main( int argc, char** argv ) {

	/* Main program - Please do not modify */

	pthread_t cpu_t, dispatcher_t, forker_t;

	printf("Initializing simulation\n");

	initialize();
	
	if ( MAIN_DEBUG ) print_pq();

	printf("Running simulation threads\n");

	if ( pthread_create(&cpu_t, NULL, (void *) cpu, NULL ) != 0)
        	perror("cpu thread failed"), exit(1); 

    	if ( pthread_create(&dispatcher_t, NULL, (void *) dispatcher, NULL ) != 0)
        	perror("dispatcher thread failed"), exit(1); 

   	if ( pthread_create(&forker_t, NULL, (void *) forker, NULL ) != 0)
        	perror("forker thread failed"), exit(1); 

    	if (pthread_join(cpu_t, NULL) != 0)
        	perror("cpu join failed"),exit(1);

    	if (pthread_join(dispatcher_t, NULL) != 0)
        	perror("dispatcher join failed"),exit(1);

    	if (pthread_join(forker_t, NULL) != 0)
        	perror("forker join failed"),exit(1);

    	printf("Simulation has terminated ... bye bye\n");

	return 0;

} // end main function

