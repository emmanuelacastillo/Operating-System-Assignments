/*
 * Emmanuel A. Castillo
 * 004268444
 * Operating Systems
 * Programming Assignment 2:
 * -----------------------------------------
 * Utilize threads in estimating the value
 * of Pi using Monte Carlo simulation. The user
 * input for the number of points will be used within
 * each thread. From there, the number of hits and the
 * number of points times the number of threads will
 * be used in the PI formula estimation.
 *
 * Compiling:
 * -----------------------------------------
 * gcc mcarlo.c -o mcarlo -lpthread -lm
 *
 * Analysis:
 * -----------------------------------------
 * The formula does a good job getting close to the value
 * of PI but not enough to be the actual value. In addition,
 * you will notice that the estimation increase with the
 * number of points used but starts to level out once
 * the numbers get very large.
 *
 * Output with argument of 37000 for the number of points.
 * -----------------------------------------
 * Input number of points is 37000.
 * 185000 number of points will be distributed among 5 threads.
 * Creating thread 0 ...
 * Creating thread 1 ...
 * Creating thread 2 ...
 * Creating thread 3 ...
 * Creating thread 4 ...
 * Thread finished. Current hit count is: 115510
 * Completed join with thread 0
 * Thread finished. Current hit count is: 135476
 * Completed join with thread 1
 * Thread finished. Current hit count is: 137567
 * Thread finished. Current hit count is: 139897
 * Thread finished. Current hit count is: 144019
 * Completed join with thread 2
 * Completed join with thread 3
 * Completed join with thread 4
 * Estimation of Pi is: 3.113924
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define MAX_INTEGER 99999998 /* This will prevent an overflow error */
#define NUM_OF_THREADS 5

int hit_count = 0;

/* Generates a double precision random number */
double random_double()
{
	return random() / ((long double)RAND_MAX + 1);
}

/* Uses Monte Carlo Simuluation to obtain a number of hits within a circle */
void *monteCarloSimulation(void *ptr)
{
    int npoints = (int) ptr, i;
    long double x, y;

    /* Check for points inside circle */
    for (i = 0; i < npoints; i++) {
        /* generate random numbers between -1.0 and +1.0 (exclusive) */
        x = random_double() * 2.0 - 1.0;
        y = random_double() * 2.0 - 1.0;

        if (sqrt(x*x + y*y) < 1.0 )
            ++hit_count;
    }

    printf("Thread finished. Current hit count is: %d \n", hit_count);

    pthread_exit(0);
}

/* Uses Monte Carlo Simulation results to estimate PI */
void calculatePi(int totalnpoints)
{
    long double pi = (long double) 4 * (long double) hit_count / (long double) totalnpoints;

    printf("Estimation of Pi is: %Lf \n", pi);
}

int main(int argc, char **argv)
{
    int npoints, i, argumentIndex = 0;
    pthread_t tid[NUM_OF_THREADS];

    /* Get the comand line parameters */
    if (argc != 2 ) {
       printf("Usage: %s <npoints> \n", argv[0]);
       exit( -1);
    }

    /* Verifies a positive integer argument is inputted. */
    while(argv[1][argumentIndex])
    {
        if(!isdigit(argv[1][argumentIndex]) || ispunct(argv[1][argumentIndex])) {
            printf("Error: Argument must be a positive integer.\n");
            return -1;
        }
        argumentIndex++;
    }

    npoints = atoi(argv[1]);

    /* Make sure input integer is not over the limit */
    if(npoints > MAX_INTEGER) {
        printf("Error: Integer is too large.\n");
        return -1;
    }

    printf("Input number of points is %d. \n", npoints);
    printf("%d number of points will be distributed among %d threads. \n", NUM_OF_THREADS * npoints, NUM_OF_THREADS);

    /* Create each thread */
    for (i = 0; i < NUM_OF_THREADS; i++)
    {
        printf("Creating thread %d ... \n", i);
        pthread_create(&tid[i], NULL, &monteCarloSimulation, (void *)npoints);
    }

    /* Wait for each thread to complete */
    for (i = 0; i < 5; i++)
    {
        pthread_join(tid[i], NULL);
        printf("Completed join with thread %d \n", i);
    }

    /* Calculates PI based on Monte Carlo Simulation results. */
    calculatePi(NUM_OF_THREADS * npoints);

    exit(0);
}
