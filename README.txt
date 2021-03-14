#SYSC 4001 - Assignment 1
@Author - Paul Warhurst
@Student Number - 101 087 869

To compile program run make file and execute <./MMULT>

##### METHOD AND DISCCUSSION #####

# Goals of Design:

- Sort 7 character array so all numbers are to the left
- Do this using three child processes, each monitoring a section
- Control the two critical sections array[2] and array[4] with two Semaphores
- Exit program as soon as all processes have registered that no more swaps are necessary
- Print out sorted array

# Method: 
-	Parent process initializes by creating a shared memory space and copying the users input into an array in this shared space. 
-	The parent process “forks” three child processes. The number of this process deicides the lower and upper limits that process can work on.
-	The parent process pauses until the four children are completed. In this case it slept as there was not enough time to integrate changes.
-	The child process makes appropriate switches
-   Semaphores are initialized and used to control the critical section of the program
-	The parent the final sorted array.


##### RESULTS #####

- Did not have enough time to complete code. 
- Using while loop to control processes ran to infinite so was replaced with a for loop that ran every process 10 times
- This was inefficient, it was anticipated to used shared_stuff -> status_array[] to keep track of when to exit
    however, I ran out of time.

Error occurred in final run, not enough time to debug

Output for compulsory data: a, m, w, 5, z, 9, 6

Suspicion is a race condition that means one of the process terminates before fully sorted
