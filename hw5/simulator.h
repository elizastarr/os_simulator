// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// simulator.h header file
// 
// Homework 5 - OS Simulator
//
// -----------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Constants
#define EMPTY 0
#define ZERO 0
#define OK 1
#define FAIL 0
#define DEBUG 1
#define UNK -1
#define TRUE 1

#define INITIAL_JOBS 15
#define MAX_JOBS 1000
#define MIN_SERVICE_TIME 5
#define MAX_SERVICE_TIME 50
#define RAND_SEED 1010		// TODO: May change this seed constant

typedef struct job_t job_t;

struct job_t {
	int e_time;		// execution time
	int s_time;		// service time
	int srt;		// shortest remaining time (srt) - page 405 course textbook
	job_t* next;	// next job in the singly linked list
};

// global variables
static job_t* cpu_job;	// job being executed by cpu
static int pq_size;		// total number of jobs in the priority queue

// NOTE: The underlying data structure used to implement 
//       the priority queue is a singly linked list ( or 
//       linked list for short) data structure.
//	
//		 The head element in the priority queue will
//		 always be the job_t with the minimum srt value.

static job_t* pq_head; // head (first element) of the priority queue
static job_t* pq_tail; // tail (last element) of the priority queue

// ----------------------------------------------------------
// TODO: Define your pthread POSIX locking (mutex or semaphore) global variables 
// 
// Use man pages to read up on sem_t, sem_init, sem_post, sem_wait, 
// pthread_mutex_t, pthread_mutex_init, pthread_mutex_lock, 
// pthread_mutex_unlock, etc.








// ------------------------------------
// Function prototype that initializes
// the simulator.
// 
//
// Arguments:	none
//
// Return:     	none
//
void initialize();

// ------------------------------------
// Function prototype that simulates an OS
// scheduler. The scheduler is a priority 
// queue, specifically a min-heap, that uses
// the srt metric
// 
//
// Arguments:	none
//
// Return:     	noe
//
void scheduler();

// ------------------------------------
// Function prototype that simulates an OS
// dispatcher. In particular, the dispatcher
// removes the job at the front of the priority
// queue and then hands it off to the CPU
// 
//
// Arguments:	none
//
// Return:     	none
//
void dispatcher();

// ------------------------------------
// Function prototype that simulates a CPU. 
// The sole job of the CPU is to execute at 
// one time quantum (q) intervals. At each 
// q the job_t execution time (e_time field) 
// is increment by one.
// 
//
// Arguments:	none
//
// Return:     	none
//
void cpu();

// ------------------------------------
// Function prototype that simulates the OS 
// creating a new job
//
// Arguments:	none
//
// Return:     	none
//
void forker();


// ------------------------------------
// Function prototype to sleep a defined
// number of nanoseconds
//
// Arguments:	number of nanosecs to sleep
//
// Return:     	none
//
void nsleep( long nanosec );


// ------------------------------------
// Function prototype that walks the linked 
// list and returns the job_t element at 
// the specified index position in the linked
// list.
//
//
// Arguments:	linked list index position
//
// Return:     	job_t pointer	-> Valid index position
//				NULL			->	Not a valid index position
//
job_t* get_list_element_( int index_position );

// ------------------------------------
// Function prototype that prints all the 
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
void print_pq();
