/*
 * Emmanuel A. Castillo
 * 004268444
 * Operating Systems
 *
 * Programming Assignment 5: 
 * Virtual Address Translator
 * -----------------------------------------
 * This program shows how a virtual address
 * are mapped into a physical address based
 * on a paging based memory management system.
 *
 * Compiling:
 * -----------------------------------------
 * gcc vmexp.c -o vmexp
 *
 * Output with argument of <address>: 12321
 * -----------------------------------------
 * The address 12321 contains:
 * page number = 3
 * offset = 33
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    /* Command line index incremeter */
    int argumentIndex = 0;

    /* Variables to handle address translation */
    unsigned long address;
    unsigned long page;
    unsigned long offset;


    /* Verify the comand line parameters */
    if (argc != 2 ) {
       printf("Usage: %s <address> \n", argv[0]);
       exit( -1);
    }

    /* Verify the address value is a valid decimal */
    while(argv[1][argumentIndex])
    {
        if(!isdigit(argv[1][argumentIndex])) {
            printf("Error: <address> must be a non-negative decimal.\n");
            exit( -1);
        }
        argumentIndex++;
    }

    /* Set address variable from input into correct data type */
    address = atoll(argv[1]);

    /* Obtain both page and offset from input address */
    page = address >> 12; // Page size is based on 4KB, thus 12 bits
    offset = address & 0xfff; // Rest of the bits is the offset

    /* Dispay results to console */
    printf("The address %lu contains: \n", address);
    printf("page number = %lu\n", page);
    printf("offset = %lu\n", offset);

    return 0;
}
