// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// simulator.c file
// 
// Homework 5 - OS Simulator
//
// -----------------------------------

#include "simulator.h"

// ------------------------------------
// Function that initializes the simulator
// 
// Arguments:	none
//
// Return:     	none
//
void initialize() {

	int i;
	job_t* job;

	srand( RAND_SEED );

	pq_size = EMPTY;

	// ----------------------------------------------
	// Add INITIAL_JOBS to the linked list 
	//
	// Please do not modify

	job = malloc( sizeof( job_t ) );
	job->s_time = MIN_SERVICE_TIME + rand() % (MAX_SERVICE_TIME-MIN_SERVICE_TIME+1); //what is needed
	job->e_time = ZERO; //how long has ran
	job->srt = ( job->s_time - job->e_time );

	pq_head = job;

	for ( i=0; i<INITIAL_JOBS-1; i++ ) { //INITIAL_JOBS = 15

		job->next = malloc( sizeof( job_t ) );
		job = job->next; //point "job" at newly created job
		job->s_time = MIN_SERVICE_TIME + rand() % (MAX_SERVICE_TIME-MIN_SERVICE_TIME+1);
		job->e_time = ZERO;
		job->srt = ( job->s_time - job->e_time );

		pq_size++;

	}

	pq_tail = job; 
	pq_tail->next = NULL;


	// ----------------------------------------------
	// TODO: Initialize your locking variables here

	sem_init(&pq_full, 0, MAX_JOBS - INITIAL_JOBS); //semWait to add, semSignal to remove
	sem_init(&pq_empty, 0, INITIAL_JOBS); //semSignal to add, semWait to remove

	sem_init(&flip_dis, 0, 1); //order disp and cpu
	sem_init(&flop_cpu, 0, 0); //order disp and cpu
	sem_init(&add_job, 0, 1);

} // end initialize function

// ------------------------------------
// Function that simulates an OS scheduler. 
// The scheduler is a priority queue, 
// specifically a min-heap that uses the 
// srt metric
// 
//
// Arguments:	none
//
// Return:     	none
//
void scheduler() {

	// ----------------------------
	// TODO: Basic scheduling algorithm
	// ----------------------------
	// Very simple, run min-heapify on 
	// linked list to create priority 
	// queue 
	// indexed from [1...n]

	int n = pq_size + 1;
   
	for( int i= floor( n/2 ) ; i>=1 ; i-- ) {
		//fprintf(stderr, "\ni= %d", i);
		int k = i;
		int heap = 0; //false

		while( !heap && 2*k <= n ){
			//fprintf(stderr, " parent_str = %d ", get_list_element_(k)->srt);
			//fprintf(stderr, "2*k=%d ", 2*k);
			int j = 2*k;

			if(j < n) { //2 children
				//fprintf(stderr, " two children: %d and %d ", get_list_element_(j)->srt, get_list_element_(j+1)->srt);
				if( get_list_element_(j+1)->srt < get_list_element_(j)->srt ){
					//fprintf(stderr, " change j ");
					j = j+1;
				}
			}

			if( get_list_element_(k)->srt <= get_list_element_(j)->srt ){ // parent < child
				//fprintf(stderr, " ALREADY a heap! ");
				heap = 1;
			}
			else { //swap
			// https://www.geeksforgeeks.org/swap-nodes-in-a-linked-list-without-swapping-data/

				job_t* currX = get_list_element_(k);
				job_t* currY = get_list_element_(j);

				if(k+1 != j) { // if they are not adjacent to eachother

					job_t* preX;
					job_t* preY = get_list_element_( j-1 );

					//fprintf(stderr, "swap: [%d]=%d and [%d]=%d, ", k, currX->srt, j, currY->srt);
					//fprintf(stderr, "[%d]=? -> [%d]=%d ... [%d]=%d -> [%d]=%d", k-1, k, currX->srt, j-1, preY->srt, j, currY->srt);
					
					if ( k != 1 ) {
						preX = get_list_element_( k-1 );
						preX->next = currY;
					}
					else { // Else make y as new head 
						pq_head = currY; 
					}  
					
					// If y is not tail of linked list 
					if ( j < n ) {
						preY->next = currX; 
					}
					else if (j == n) { // Else make x new tail
						preY->next = currX;
						pq_tail = currX; 
					}

					// Swap next pointers 
					job_t* temp = currY->next; 
					currY->next = currX->next; 
					currX->next  = temp; 

					if(j==n)
						pq_tail->next = NULL;
				}
				else { // they are adjacent
					job_t* preX;

					//fprintf(stderr, "swap: [%d]=%d and [%d]=%d, ", k, currX->srt, j, currY->srt);
					//fprintf(stderr, "[%d]=? -> [%d]=%d ... [%d]=? -> [%d]=%d", k-1, k, currX->srt, j-1, j, currY->srt);
					
					if ( k != 1 ) {
						preX = get_list_element_( k-1 );
						preX->next = currY;
					}
					else {// Else make y as new head 
						pq_head = currY; 
					}  
					
					if ( j == n ) {// Make x new tail
						pq_tail = currX; 
					}

					// Swap next pointers 
					job_t* temp = currY->next; 
					currY->next = currX; 
					currX->next  = temp; 

					if(j==n)
						pq_tail->next = NULL;
				}

			}
			
		}

	}

} // end scheduler function


// ------------------------------------
// Function that simulates an OS
// dispatcher. In particular, the dispatcher
// removes the job at the front of the priority
// queue and then hands it off to the CPU
// 
//
// Arguments:	none
//
// Return:     	none
//
void dispatcher() {

	// -------------------------
	// concurrency constraints
	// -------------------------
	// cannot remove a job from priority queue:
	// 	- if it is empty
	// 	- if cpu or forker is adding a job
	
	while ( TRUE ) {

		sem_wait(&flip_dis); // cpu not running

		/*
		fprintf(stderr, "\n-------------------------------------------\nENTER DISPATCHER");
		int temp1;
		sem_getvalue(&pq_empty, &temp1);
		fprintf(stderr, " pq_empty value before decrement: %d ", temp1);
		int temp;
		sem_getvalue(&pq_full, &temp);
		fprintf(stderr, "pq_full value: %d \n", temp);
		*/

		sem_wait(&pq_empty); // is pq empty? If so, wait until forker adds a job.
		sem_wait(&add_job); // is forker running? If not, run dispacher


		// --------------------------------
		// TODO: Basic algorithm
		// --------------------------------
		// 1. run scheduler to identify next priority queue job
		
		scheduler();
		//print_pq();

		// 2. remove job from priority queue (update pq_size, a global var)

		pq_size--;
		cpu_job = pq_head; 
		pq_head = cpu_job->next; //don't want cpu running

		sem_post(&add_job); // ok for forker and/or cpu to run next

		// 3. hand job off to cpu (having assigned new cpu_job)
		sem_post(&flop_cpu);

	}

} // end dispatcher function

// ------------------------------------
// Function that simulates a CPU. 
// The sole job of the CPU is to execute at 
// one time quantum (q) intervals. At each 
// q the job_t execution time (e_time field) 
// is incremented by one.
// 
//
// Arguments:	none
//
// Return:     	none
//
void cpu() {

	// -------------------------
	// concurrency constraints
	// -------------------------
	// cannot add job to priority queue:
	// 	- 1st: if it is full
	// 	- if dispatcher is running scheduler to remove next job
	// 	- if forker is adding a job
	//fprintf(stderr, "CPU");

	
	while ( TRUE ) {

		sem_wait(&flop_cpu); // time for cpu to run
		//fprintf(stderr, " \nENTER CPU ");

		// --------------------------------
		// TODO: Basic algorithm
		// --------------------------------
		// 1. run cpu job for only one time quantum (nsleep)
		//    and then update e_time++ and srt fields--

		nsleep(1);
		cpu_job->e_time += 1;
		cpu_job->srt -= 1;


		// 2.a. if cpu_job is not finished (srt>0) 
		//      add to priority queue (update pq_size)
		
		if(cpu_job->srt > 0){ // add job back to pq

			sem_wait(&add_job);

			/*
			fprintf(stderr, " PUT CPU_JOB BACK IN PQ \n");
			int temp1;
			sem_getvalue(&pq_empty, &temp1);
			fprintf(stderr, "pq_empty value before increment: %d ", temp1);
			*/

			pq_size++;

			/*
			cpu_job->next = NULL;
			pq_tail->next = cpu_job;
			pq_tail = cpu_job;
			*/
			cpu_job->next = pq_head;
			pq_head = cpu_job;

			sem_post(&add_job);
			sem_post(&pq_empty); // added to pq

		}


		// 2.b. if cpu_job is finished (srt==0)
		//      free job_t malloc'd memory

		else {// srt == 0
			//fprintf(stderr, " FREE CPU JOB \n");
			free(cpu_job);

			/*
			int temp;
			sem_getvalue(&pq_full, &temp);
			fprintf(stderr, "pq_full value before increment: %d ", temp);
			*/

			sem_post(&pq_full);

			
		}

		sem_post(&flip_dis); // ok for dispacher to run

	}
	
	
} // end cpu function


// ------------------------------------
// Function that simulates the OS 
// creating a new job
//
// Arguments:	none
//
// Return:     	none
//
void forker() {

	// -------------------------
	// concurrency constraints
	// -------------------------
	// cannot add job to priority queue:
	// 	- if it is full
	// 	- if dispatcher is running scheduler to remove next job
	// 	- if cpu is adding a job

	
	while ( TRUE ) {

		// --------------------------------
		// TODO: Basic algorithm
		// --------------------------------
		
		// 1. Create a new job
		//    - s_time is a random number in [MIN_EXE_TIME MAX_EXE_TIME]
		//    - s_time is intialized to zero
		//	  - see initialize function for an example


		srand( RAND_SEED );

		job_t* job = malloc( sizeof( job_t ) );
		job->s_time = MIN_SERVICE_TIME + rand() % (MAX_SERVICE_TIME-MIN_SERVICE_TIME+1); //what is needed
		job->e_time = ZERO; //how long has ran
		job->srt = ( job->s_time - job->e_time );
		job->next = NULL;

		// 2. add job to pq (update list_size)

		/*
		int temp;
		sem_getvalue(&pq_full, &temp);
		fprintf(stderr, "TRY FORKER.  pq_full before decrement: %d ", temp);
		*/

		sem_wait(&pq_full); 
		sem_wait(&add_job);

		pq_size++;
		pq_tail->next = job; // point the current tail to job
		pq_tail = job; // the new tail is job

		sem_post(&add_job);
		sem_post(&pq_empty); // added to pq

		//fprintf(stderr, " FORKER ADDED \n ------------------------------\n");
		//print_pq();


		// 3. sleep for nanoseconds (you determine)
		nsleep(1000000000);
		

	}

} // end forker


// ------------------------------------
// Function that walks the linked 
// list and returns the job_t element at 
// the specified index position in the linked
// list.
//
//
// Arguments:	linked list index position
//
// Return:     	job_t pointer	-> Valid index position
//				NULL 			->	Not a valid index position
//
job_t* get_list_element_( int index_position ) {

	// --------------------------------
	// TODO: Add code here
	// --------------------------------
	
	if (index_position <= pq_size + 1) { // it can be found
		job_t* current = pq_head;
		int count = 0; 
		while (current != NULL) 
		{ 
			// index_position comes from a range of [1,n], not [0,n]
			if (count == index_position - 1) 
				break; 
			count++;
			current = current->next; 
		} 
		return current;
	}
	else {
		return 0;
	}

	

} // end get_list_element function


// ------------------------------------
// Function prototype to sleep a defined
// number of nanoseconds
//
// Arguments:	number of nanosecs to sleep
//
// Return:     	none
//
void nsleep( long nanosec ) {

	// Please do not modify this code

    	struct timespec t_spec;
        
    	t_spec.tv_sec = 0;
    	t_spec.tv_nsec = nanosec;

   	nanosleep( &t_spec, NULL );
        
} // end nsleep function


// ------------------------------------
// Function that prints all the 
// job_t elements in the priority queue
//
// Used for debugging purposes only
//
// May modify as you see fit
//
// Arguments:	none
//
// Return:     	none
//
void print_pq() {

	int count = 0;

	job_t* job = pq_head;

	//int temp1;
	//sem_getvalue(&pq_full, &temp1);
	//printf("Initial value of pq_full: %d ", temp1);


	if ( DEBUG ) printf("Total number of jobs in priority queue = %d\n", pq_size + 1 );

	while ( job != NULL  && count < pq_size +10) {

		if ( DEBUG ) {

			printf("--------------------------\n");
			printf("job_t[%d]->e_time = %d\n", ++count, job->e_time );
			printf("job_t[%d]->s_time = %d\n", count, job->s_time );
			printf("job_t[%d]->srt = %d\n", count, job->srt );

		}

		job = job->next;

	}

} // end print_pq function
