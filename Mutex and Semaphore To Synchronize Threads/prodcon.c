/*
 * Emmanuel A. Castillo
 * 004268444
 * Operating Systems
 * Programming Assignment 3:
 * -----------------------------------------
 * Synchronize threads in accessing data
 * from a memory block using mutex and
 * counting semaphores. A seperate empty and full
 * counting semaphore is used to determine whether
 * a producer has space to write or a consumer
 * has data to read. The mutex implements mutual
 * exclusion among the shared memory block
 * so the producer and consumer can properly
 * synchronize their write and read
 * operations, respectively. Additionally,
 * a checksum is used on the last 2 bytes of
 * each 32 bytes block to validate the data.
 *
 * Compiling:
 * -----------------------------------------
 * gcc prodcon.c -o prodcon -lpthread -lm
 *
 *
 * Output with argument of <memsize>: 640, <ntimes>: 7.
 * -----------------------------------------
 * PRODUCER: written value: g�isQ�J�)ͺ����F|�T���vZ.c3�)�)�
 * �1X�Z%]X�^ԫ��ƛ�T�tA�� f2
 * PRODUCER: written value: !=܇p�>�A��g>~���k�\*��;�2�����
 * PRODUCER: written value: <T��\�C���:�)��<|�uؾa��\�����
 * PRODUCER: written value: �������������
 * PRODUCER: written value: d�sZ�^Kyc;pd$�	ܪԬ�;3��P``
 * PRODUCER: written value: HG\�o"��}�
 *                                    �#�)��N��2>Z>Z
 * CONSUMER: read value: g�isQ�J�)ͺ����F|�T���vZ.c3�)�
 * �1X�Z%]X�^ԫ��ƛ�T�tA� 2
 * CONSUMER: read value: !=܇p�>�A��g>~���k�\*��;�2���
 * CONSUMER: read value: <T��\�C���:�)��<|�uؾa��\���
 * CONSUMER: read value: �����������
 * CONSUMER: invalid checksum detected.
 * CONSUMER: read value: d�sZ�^Kyc;pd$�	ܪԬ�;3��P`
 * CONSUMER: read value: HG\�o"��}�
 *                                 �#�)��N��2>Z
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

#define MEM_SIZE_MAX 64000

pthread_t tid[2];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

char **SHARED_MEMORY;
unsigned int ntimes;

char *generateRandom30bytes()
{
    char randomByte, *bytes = malloc(30);
    int count;

    /* Generates random 30 bytes and puts into char* bytes */
    for(count = 0; count < 30; count++)
    {
        randomByte = random();
        *(bytes + count) = randomByte;
    }

    return bytes;
}

char *generateCheckSum(char *bytes)
{
    uint32_t checkSumCalc = 0;
    size_t byteSize = 30;
    uint16_t checkSumCalc16bits;
    char *checkSum = malloc(2);

    while(byteSize > 1)
    {
        checkSumCalc = checkSumCalc + *((uint16_t *) bytes);
        bytes = bytes + 2;
        byteSize = byteSize - 2;
    }

    /* Add left-over byte, if any */
    if (byteSize > 0)
        checkSumCalc = checkSumCalc + *((char *) checkSumCalc);

    /* Fold 32-bit sum to 16 bits */
    while (checkSumCalc>>16)
        checkSumCalc = (checkSumCalc & 0xFFFF) + (checkSumCalc >> 16);

    checkSumCalc = ~checkSumCalc;

    checkSumCalc16bits = (uint16_t)checkSumCalc;

    /* Set check sum into char* of 2 bytes */
    memcpy(checkSum, (char *)&checkSumCalc16bits, 2);

    return checkSum;
}

void *producer(void *arg)
{
    char *randomBytes, *checkSum;
    int producedDataIndex;

    /* Write data to memory for consumer to read. */
    for(producedDataIndex = 0; producedDataIndex < ntimes; producedDataIndex++)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        /* Generates random 30 bytes and check sum
        for the full 32 bytes for a memory block */
        randomBytes = generateRandom30bytes();
        checkSum = generateCheckSum(randomBytes);
        strcat(randomBytes, checkSum);

        /* Copy data into memory */
        strncpy(SHARED_MEMORY[producedDataIndex], randomBytes, 32);

        printf("PRODUCER: written value: %s%s \n", randomBytes, checkSum);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }

    /* Clear resources */
    free(randomBytes);
    free(checkSum);

    return NULL;
}

void *consumer(void *arg)
{
    char *bytesForCheckSum = malloc(30), *actualCheckSum, *expectedCheckSum = malloc(2);
    int readDataIndex;

    /* Read data from memory when producer finishes producing data. */
    for(readDataIndex = 0; readDataIndex < ntimes; readDataIndex++)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        printf("CONSUMER: read value: %s \n", SHARED_MEMORY[readDataIndex]);

        /* Verifies data integrity through check sum */
        memcpy(bytesForCheckSum, SHARED_MEMORY[readDataIndex], 30);
        actualCheckSum = generateCheckSum(bytesForCheckSum);
        memcpy(expectedCheckSum, SHARED_MEMORY[readDataIndex] + 30, 2);
        if(strcmp(actualCheckSum, expectedCheckSum) != 0) {
            printf("CONSUMER: invalid checksum detected. \n");
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

    /* Clear resources */
    free(bytesForCheckSum);
    free(actualCheckSum);
    free(expectedCheckSum);

    return NULL;
}

int main(int argc, char **argv)
{
    int argumentIndex = 0, memsize, err;
    size_t numOfBlocks, blockIndex;

    /* Get the comand line parameters */
    if (argc != 3 ) {
       printf("Usage: %s <memsize> <ntimes> \n", argv[0]);
       exit( -1);
    }

    /* Verifies a positive integer argument is inputted for memsize. */
    while(argv[1][argumentIndex])
    {
        if(!isdigit(argv[1][argumentIndex])) {
            printf("Error: <memsize> must be a positive integer.\n");
            return -1;
        }
        argumentIndex++;
    }

    /* Verifies a positive integer argument is inputted for ntimes. */
    argumentIndex = 0;
    while(argv[1][argumentIndex])
    {
        if(!isdigit(argv[1][argumentIndex])) {
            printf("Error: <ntimes> must be a positive integer.\n");
            return -1;
        }
        argumentIndex++;
    }

    /* Initialize memsize and ntimes from input arguments */
    memsize = atoi(argv[1]);
    ntimes = atoi(argv[2]);

    /* Verifies memsize is a below its max value. */
    if(memsize > MEM_SIZE_MAX) {
            printf("Error: <memsize> must be less than %d.\n", MEM_SIZE_MAX);
            return -1;
    }

    /* Verifies memsize is a multiple of 32. */
    if((memsize % 32) != 0) {
            printf("Error: <memsize> must be a multiple of 32.\n");
            return -1;
    }

    /* Verifies ntimes is within number of memory blocks */
    if(ntimes > (memsize/32)) {
        printf("Error: Integer is too large.\n");
        return -1;
    }

    /* Allocate shared memory region based on memsize where each block is 32 bits */
    numOfBlocks = memsize/32;
    SHARED_MEMORY = (char**)malloc(numOfBlocks * sizeof(char *));
    for(blockIndex = 0; blockIndex < numOfBlocks; blockIndex++)
    {
        SHARED_MEMORY[blockIndex] = (char *)malloc(32*sizeof(char ));
    }

    /* Create the mutex to protect the critical section */
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n error creating mutex\n");
        return -1;
    }

    /* Create the counting semaphore for producer to fill up memory blocks */
    if(sem_init(&empty, 0, numOfBlocks) != 0) {
        printf("\n error creating semaphore\n");
        return -1;
    }

    /* Create the counting semaphore for consumer to obtain the data from memory blocks*/
    if(sem_init(&full, 0, 0) != 0) {
        printf("\n error creating semaphore\n");
        return -1;
    }

    /* Create producer and consumer threads */
    err = pthread_create(&(tid[0]), NULL, &producer, NULL);
    if (err != 0) {
        printf("\ncan't create thread 1 :[%s]", strerror(err));
    }

    err = pthread_create(&(tid[1]), NULL, &consumer, NULL);
    if (err != 0) {
        printf("\ncan't create thread 2 :[%s]", strerror(err));
    }

    /* Wait for threads to be finished */
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    /* Clear resources */
    free(SHARED_MEMORY);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}

