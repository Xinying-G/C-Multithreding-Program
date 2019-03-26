// C language thread programming:

// Comp111 Assignment 2: ants and aardvarks
// Overview
// One of the most famous problems in concurrency is that of the "Dining Philosophers", who must share knives and forks in a concurrent manner. But this has been studied too much in the literature, so instead, we will study the "Dining Aardvarks".
// There are 11 aardvarks and 3 anthills. There are 100 ants in each anthill. Only 3 aardvarks can share one anthill at a time. It takes one second of real time for an aardvark to slurp up an ant, and another second for the aardvark to swallow it, during which time it is not using the anthill and another aardvark can start slurping. However, should an aardvark attempt to slurp an ant from an anthill where 3 aardvarks are already slurping, or make any other kind of mistake, including attempting to slurp from an already empty anthill or an anthill that doesn't exist, it will be 4 seconds before the aardvark is available to slurp again. The simulator calls that 'sulking'.
// Objectives
// Your objective is to manage the aardvarks so that all of the ants are consumed in a minimal amount of real time. Each aardvark is a thread in a multi-threaded program. You are given a simulator for the anthills that invokes the aardvarks and measures the resulting behavior.
// You must write one thread
 
// extern void *aardvark(void *); 
// that is invoked 11 times. This thread should simulate the behavior of one aardvark so that 11 instances of this thread solve the problem.
// When invoked, the thread has one argument passed to it, which is the name of the aardvark that it is simulating. The bare skeleton of a thread is something like:
 
// int initialized=0; 
// void *aardvark(void *input) {
//     char aname = *(char *)input; // name of aardvark, for debugging purposes
//     pthread_mutex_lock(&init_lock); // declared in anthills.h
//     if (!initialized++) { // this is executed for only one thread.
//     // initialize all variables, mutexes, semaphores here
//     } 
//     pthread_mutex_unlock(&init_lock); 
//     while (chow_time()) {  // there is an ant to eat
//     // try to slurp an ant!
//     slurp(aname); 
//     }
//     return NULL;
// }
// Each thread may utilize the following interface, documented in /comp/111/a/a2/anthills.h :
 
// #define TRUE 1
// #define FALSE 0 
// #define AARDVARKS 11
// #define ANTHILLS 3
// #define ANTS_PER_HILL 100
// #define AARDVARKS_PER_HILL 3
// extern int slurp(char aname, int anthill); // eat one ant. 
// extern int chow_time(); // whether there are ants to eat
// extern double elapsed(); // how much time has been spent? 
// extern pthread_mutex_t init_lock; // resolve init race conditions 
//     •   slurp tries to slurp up an ant. It returns in two seconds of real time if it succeeds, and 4 seconds of real time if it does not. During the second second of a successful slurp, the hill can be slurped again. However, the simulator "believes" the thread about what constitutes the identity of his aardvark. One aardvark will not be allowed to slurp from two anthills at the same time, if another thread "impersonates" the same aardvark by giving the wrong name. slurp returns 1 if it succeeds (and the aardvark actually consumes an ant) and 0 if it fails.
//     •   chow_time returns 1 if there's an ant left anywhere, and 0 if not. Your threads should return when this returns 0.
//     •   elapsed returns the seconds of real time (wallclock time) since the start of the simulation.
// A starting version of the aardvarks code may be found in /comp/111/a/a2/aardvarks.c.
// The Simulator
// You will test your aardvarks.c by use of a simulator, currently contained in /comp/111/a/a2/anthills.c and /comp/111/a/a2/anthills.h. You may not modify this simulator (except to test your theories), but may use whatever information you can glean from the source code. During grading, this simulator will be the one that is used. The simulator creates your threads and waits for them to complete. It also provides mechanisms by which your threads must slurp the ants. These mechanisms employ concurrency locks in the manner discussed in class: see the code for details.

// Getting started
// To get started,
// mkdir a2
// cd a2 
// cp /comp/111/a/a2/aardvarks.c .
// ln -s /comp/111/a/a2/anthills.o .
// gcc -g aardvarks.c anthills.o -lpthread -lrt 
// ./a.out


#include "/comp/111/a/a2/anthills.h" 
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

int initialized=FALSE; // semaphores and mutexes are not initialized 

sem_t anthill[3];
int slurping[ANTHILLS]= {100,100,100};

// A thread that can post semaphores
void *post(void *input){
    int sem = *(int *)input;
    usleep(1.05e6);
    sem_post(&anthill[sem]);
}

// first thread initializes mutexes 
void *aardvark(void *input) { 
    char aname = *(char *)input; 
    // first caller needs to initialize the mutexes!
    pthread_mutex_lock(&init_lock); 
    if (!initialized++) { // this succeeds only for one thread initialiize your mutexes and semaphores here
        sem_init(&anthill[0],0,3);
        sem_init(&anthill[1],0,3);
        sem_init(&anthill[2],0,3);
    } 
    pthread_mutex_unlock(&init_lock); 

    // now be an aardvark
    while (chow_time()) { 
        int i = rand()%3;
        // If the thread get semaphore, then eat
        if(!sem_trywait(&anthill[i]))
        {
            // make sure there are ants in the anthill
            if(slurping[i] > 0)
            {
                //slurping
                pthread_mutex_lock(&init_lock);
                slurping[i]--; 
                pthread_mutex_unlock(&init_lock);

                pthread_t thread;
                pthread_create(&thread, NULL, 
                        post, (void *)(&i));
                // If failed eating, add the number of ant back 
                if(!slurp(aname,i))
                {
                    pthread_mutex_lock(&init_lock);
                    slurping[i]++;
                    pthread_mutex_unlock(&init_lock);
                }
            }        
        }

    } 

    return NULL; 
}




