#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/sem.h>

#include "shm_com.h"
#include "semun.h"
#include "array_functions.h"

static int set_semvalue(int i);
static void del_semvalue(int i);
static int semaphore_p(int i);
static int semaphore_v(int i);

static int sem_id;

// Creating shared memory

int main()
{
    printf("INSTRUCTIONS:\n-> Enter 7 alphanumeric or alphabetic characters.\n-> If more than 7 characters are entered, the rest will be ignored.\n");
    printf("-> Do NOT seperate characters with spaces or operators.\n");
    char buffer[BUFSIZ];
    int num_char = 0;
    int debug = 0;

    /* Set up shared memory to hold array */
    void *shared_memory = (void *) 0;
    struct shared_use_st *shared_stuff;     // Mask for shared memory

    int shmid;
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    /* Handle shared memory errors */
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1){
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    shared_stuff = (struct shared_use_st *) shared_memory;

    /* Create two semaphores */
    sem_id = semget((key_t)1235, 2, 0666 | IPC_CREAT);
    /* Initialise two semaphores */
    if(!set_semvalue(0) | !set_semvalue(1)) {
        fprintf(stderr, "Failed to initialise semaphore \n");
        exit(EXIT_FAILURE);
    }
    
    /* Handling user input */
    printf("Enter 7 character array: "); 
    fgets(buffer, BUFSIZ, stdin);
    copy_array(buffer, shared_stuff->filtered, debug);

    printf("Run in debug mode? Enter 'yes' for yes: ");
    fgets(buffer, BUFSIZ, stdin);
    if(strncmp(buffer, "yes", 3) == 0) {
        debug = 1;
    }

    /* Summarise inputs */
    printf("Unsorted array entered: ");
    print_array(shared_stuff->filtered);
    if(debug == 1){
        printf("Running in debug mode.\n");
        printf("Memory attached at %X\n", shared_memory);
    }

    /* Set status array to zeros */
    shared_stuff->status_array[0] = 0;
    shared_stuff->status_array[1] = 0;
    shared_stuff->status_array[2] = 0;

    /* Fork 3 children */
    pid_t pid;
    for(int p = 0; p < 3; p++){
        pid = fork();
        switch(pid){
            case -1:
                perror("Fork failed\n");
                exit(EXIT_FAILURE);
            case 0:
                if(debug == 1) printf("Child number %d successfully forked\n", p);
                /* Set lower and upper limits each process can work on */
                int lower_limit, upper_limit;
                if(p == 0){
                    lower_limit = 0;
                    upper_limit = 2;
                }
                else if(p == 1){
                    lower_limit = 2;
                    upper_limit = 4;
                }
                else {
                    lower_limit = 4;
                    upper_limit = 6;
                }

                /* for loop that cycles through array */
                int switched = 0;
                for(int j = 0; j < 20; j++){
                    for(int i = lower_limit; i <= upper_limit; i++){
                        if(i == 2 | i == 1){
                            if(!semaphore_p(0)) exit(EXIT_FAILURE);
                            if(debug == 1) printf("Semaphore 0 blocked\n");
                        }
                        if(i == 4 | i == 3){
                            if(!semaphore_p(1)) exit(EXIT_FAILURE);
                            if(debug == 1) printf("Semaphore 1 blocked\n");
                        }
                        if((isalpha(shared_stuff->filtered[i]) == 0) && (isalpha(shared_stuff->filtered[i+1]) != 0)){
                            if(debug == 1){
                                printf("Making switch...\nSwitched array: ");
                                print_array(shared_stuff->filtered);
                            }
                            char temp = shared_stuff->filtered[i];
                            shared_stuff->filtered[i] = shared_stuff->filtered[i + 1];
                            shared_stuff->filtered[i+1] = temp;
                            switched = 1;
                        }
                        if(i == 2 | i == 1){
                            if(!semaphore_v(0)) exit(EXIT_FAILURE);
                            if(debug == 1) printf("Semaphore 0 unblocked\n");
                        }
                        if(i == 4 | i == 3){
                            if(!semaphore_v(1)) exit(EXIT_FAILURE);
                            if(debug == 1) printf("Semaphore 1 unblocked\n");
                        }
                    }
                    if(switched == 1) shared_stuff->status_array[p] = 0;
                    if(switched != 1) shared_stuff->status_array[p] = 1;
                }
                if(debug==1) printf("Child %d terminating...\n", p);
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }


    sleep(2);
    printf("Final sorted array: ");
    print_array(shared_stuff->filtered);
    printf("\n");

    exit(EXIT_SUCCESS);

}

static int set_semvalue(int i)
{
    union semun sem_union;

    sem_union.val = 1;
    if (semctl(sem_id, i, SETVAL, sem_union) == -1) return(0);
    return(1);
}

static void del_semvalue(int i)
{
    union semun sem_union;

    if (semctl(sem_id, i, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore");
    //printf("semaphore deleted\n");
}

static int semaphore_p(int i)
{
    struct sembuf sem_b;

    sem_b.sem_num = i; 
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);
}

static int semaphore_v(int i)
{
    struct sembuf sem_b;

    sem_b.sem_num = i; 
    sem_b.sem_op = 1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);
}