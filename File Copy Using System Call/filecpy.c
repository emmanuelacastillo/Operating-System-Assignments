/*
 * Emmanuel A. Castillo
 * 004268444
 * Operating Systems
 * Programming Assignment 1:
 * -----------------------------------------------------
 * Develop a program that will use system calls to copy
 * one file to another file.
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#define REQ_ARGS 3 // Required number of program arguments
#define SRC_FILE_IDX 1 // Source file argument index
#define DST_FILE_IDX 2 // Destination file argument index
#define BUFF_SIZE 256

void error(const char *errorMsg)
{
 perror(errorMsg);
 exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
 char *srcFile, *dstFile, fileBuffer[BUFF_SIZE];
 int srcFileDesc, dstFileDesc;
 size_t srcReadBytes, dstReadBytes, dstWriteBytes, srcCloseStatus, dstCloseStatus;

 // Verifies the correct number of arguments are provided
 if(argc != REQ_ARGS)
  error("Please provide a source and destination file path as arguments.\n");

 srcFile = argv[SRC_FILE_IDX];
 srcFileDesc = open(srcFile, O_RDONLY);

 printf("Src file desc: %u\n", srcFileDesc);

 if (srcFileDesc < 0)
      error(strcat("Error opening file: ", srcFile));

   dstFile = argv[DST_FILE_IDX];
   // Either overwrites and creates an output file with read and write access.
   dstFileDesc = open(dstFile, O_WRONLY | O_APPEND | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

   printf("Dst file desc: %u\n", dstFileDesc);

   // Validates that the output file open (or create) successfully
   if (dstFileDesc < 0)
      error(strcat("Error opening file: ", dstFile));

   // Reads source file bytes and writes to destination file.
   while((srcReadBytes = read(srcFileDesc, fileBuffer, BUFF_SIZE)) > 0)
   {
      // Verifies the read operation was successful
      if (srcReadBytes < 0)
         error(strcat("Error reading file: ", srcFile));

      printf("Src read bytes: %u\n", srcReadBytes);

      dstWriteBytes = write(dstFileDesc, fileBuffer, srcReadBytes);
      
      // Verifies the write operation was successful
      if(dstWriteBytes < 0)
         error(strcat("Error writing to file: ", dstFile));

      printf("Dst write bytes: %u\n", dstWriteBytes);
   }
 
   // Closes both source and destination file for proper use later.  
   srcCloseStatus = close(srcFileDesc);

   printf("Src close status: %u\n", srcCloseStatus);

   dstCloseStatus = close(dstFileDesc);

   printf("Dst close status: %u\n", dstCloseStatus);
 
   // Verifies the close operations were successful
   if (srcCloseStatus != 0)
      error(strcat("Error closing file: ", srcFile));
   if (dstCloseStatus != 0)
      error(strcat("Error closing file: ", dstFile));

   return(EXIT_SUCCESS);
}
