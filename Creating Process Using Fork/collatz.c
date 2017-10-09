/*
 * Emmanuel A. Castillo
 * 004268444
 * Operating Systems
 * Programming Assignment 2:
 * -----------------------------------------
 * Create a child process using fork()
 * and have the parent process wait for
 * the child process to finish before proceeding.
 *
 * Output with argument of 24 (./collatz 24)
 * -----------------------------------------
 * CHILD: Current value = 12
 * CHILD: Current value = 6
 * CHILD: Current value = 3
 * CHILD: Current value = 10
 * CHILD: Current value = 5
 * CHILD: Current value = 16
 * CHILD: Current value = 8
 * CHILD: Current value = 4
 * CHILD: Current value = 2
 * CHILD: Current value = 1
 * PARENT: Process 20743 ended with status 0
 *
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define REQ_ARG_COUNT 2
#define INT_ARG 1
#define MAX_INTEGER 499999990
 
/* Method to handle actual errors */
void error(const char *errorMsg)
{
    perror(errorMsg);
    exit(EXIT_FAILURE);
}

/* Check whether the integer is even or odd */
int isEven(int value)
{
    if(value % 2 == 0)
        return 1;
    else
        return 0;
}

/* Uses Collatz Conjecture formula when integer is even */
int evenCollatzConjecture(int curValue)
{
    return curValue / 2;
}

/* Uses Collatz Conjecture formula when integer is odd */
int oddCollatzConjecture(int curValue)
{
    return 3 * curValue + 1;
}

/* Child process method: uses recursion to display Collatz Conjecture calculation */
void runChildProcess(int intToProcess)
{
    /* Loops through Collatz Conjecture until the expected end value of 1 */
    do
    {
        if(isEven(intToProcess) == 1)
            intToProcess = evenCollatzConjecture(intToProcess);
        else
            intToProcess = oddCollatzConjecture(intToProcess);

        printf("CHILD: Current value = %d \n", intToProcess);
    } while (intToProcess > 1);
}

/* Parent process method: waits for child process to end */
void runParentProcess()
{
    pid_t cmpPid;
    int status;
    cmpPid = wait(&status);

    if(status != 0) {
        /* Verifies expected return value of 0 is returned. */
        error("ERROR: Child process ended incorrectly");
    }

    printf("PARENT: Process %d ended with status %d\n", cmpPid, status);
}

int main(int argc,  char **argv)
{
    int intToProcess, argumentIndex = 0;
    pid_t childPid;

    if(argc != REQ_ARG_COUNT) {
        /* Verifies only expected number of arguments are set. */
        printf("ERROR: Invalid argument count.\n");
        return -1;
    }

    /* Verifies a positive integer argument is inputted. */
    while(argv[INT_ARG][argumentIndex])
    {
        if(!isdigit(argv[INT_ARG][argumentIndex])) {
            printf("ERROR: Argument must be a positive integer.\n");
            return -1;
        }
        argumentIndex++;
    }

    /* Converts user input into integer for processing. */
    intToProcess = atoi(argv[INT_ARG]);

    if(intToProcess > MAX_INTEGER) {
	/* Make sure input integer is not over the limit */    
	printf("ERROR: Integer is too large.\n");
	return -1;   
    }

    if ((childPid = fork()) < 0) {
       /* Fork command failed - check the error. */
       error("ERROR: fork failed.\n");
    }
    else if (childPid == 0) {
        runChildProcess(intToProcess);
    }
    else {
        runParentProcess();
    }

    return 0;
}
