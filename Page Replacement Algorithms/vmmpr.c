/*
 * Emmanuel A. Castillo
 * 004268444
 * Operating Systems
 *
 * Programming Assignment 6:
 * Page Replacement Algorithm
 * -----------------------------------------
 * This program mimics 3 different page replacement
 * algorithms: FIFO, LRU, OPTIMAL. The number of
 * page faults is displayed per algorithm. The
 * one with the least is the most efficient.
 *
 * Compiling:
 * -----------------------------------------
 * gcc vmmpr.c -o vmmpr
 *
 * Output with argument of <page references>: 2,3,1,3,2,5,7,7,2,5,1,2,5
 * -----------------------------------------
 * Page Fault Results
 * -------------------
 * FIFO: 8
 * LRU: 8
 * OPTIMAL: 6
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGE_REFS 40
#define MIN_PAGE_NUM 0
#define MAX_PAGE_NUM 9

#define FRAME_SZ 3

#define OPT_MAX_LEN 41

inline int getFifoPageFaults(char *pageRefs)
{
   int num_pagefault = 0;
   int fifo_i;
   char fifo[FRAME_SZ] = { 0 };

   /* Sets page references into frame array and counts page faults */
   while (*pageRefs)
   {
      if (*pageRefs == ',') {
         *pageRefs++;
         continue;
      }

      /* Loops through the frame array and puts the page into        *
       * a frame if it does not exist already. Every new placement   *
       * signifies a page fault.                                     */
      for (fifo_i = 0; fifo_i < FRAME_SZ; fifo_i++)
      {
         /* If page exist in frame already no page fault occurs. */
         if (fifo[fifo_i] == *pageRefs) {
            break;
         }

         /* If an empty spot exist in the frame and the page   *
          * does not exist in the frame yet, place it in and   *
          * count a page fault.                                */
         else if (fifo[fifo_i] == '\0') {
            num_pagefault++;
            fifo[fifo_i] = *pageRefs;
            break;
         }

         /* If we entered the last frame without finding the   *
          * it will replace the last frame with the page and   *
          * count a page fault.                                */
         else if ((fifo_i + 1) == FRAME_SZ) {
            num_pagefault++;
            fifo[fifo_i] = *pageRefs;
            break;
         }
      }
      *pageRefs++;
   }
   return num_pagefault;
}

typedef struct {
   int count;
   char value;
} PAGE_INFO;

int asc_sort(const void *lru_l, const void *lru_r)
{
   return (((PAGE_INFO *)lru_r)->count - ((PAGE_INFO *)lru_l)->count);
}

inline int getLruPageFaults(char *pageRefs)
{
   int num_pagefault = 0;
   int lru_i;
   PAGE_INFO lru[FRAME_SZ] = { 0 };

   /* Sets page references into frame array and counts page faults */
   while (*pageRefs)
   {
      if (*pageRefs == ',') {
         *pageRefs++;
         continue;
      }

      /* Loops through the frame array and puts the page into       *
       * a frame if it does not exist already. Every new placement   *
       * signifies a page fault. If page exist already its count     *
       * increases so it can be used to determine what is replaced   *
       * later on.                                                   */
      for (lru_i = 0; lru_i < FRAME_SZ; lru_i++)
      {
         /* If page exist in frame already no page fault occurs.  *
          * Also, its count increases.                            */
         if (lru[lru_i].value == *pageRefs) {
            lru[lru_i].count++;
            break;
         }

         /* If an empty spot exist in the frame and the page   *
          * does not exist in the frame yet, place it in and   *
          * count a page fault.                                */
         else if (lru[lru_i].value == '\0') {
            num_pagefault++;
            lru[lru_i].count = 1;
            lru[lru_i].value = *pageRefs;
            break;
         }

         /* If we entered the last frame without finding the   *
          * it will replace the last frame with the page and   *
          * count a page fault.                                */
         else if ((lru_i + 1) == FRAME_SZ) {
            num_pagefault++;
            qsort(lru, FRAME_SZ, sizeof(PAGE_INFO), asc_sort);
            lru[lru_i].count = 1;
            lru[lru_i].value = *pageRefs;
            break;
         }
      }
      *pageRefs++;
   }
   return num_pagefault;
}

inline int getOptimalReplacementIndex(char frame[], char *currentPageRefs)
{
   int opt_page_i = FRAME_SZ - 1;
   int frame_i;
   int page_i;
   int num_page_passed = 0;
   PAGE_INFO opt[FRAME_SZ] = { 0 };

   /* Set up struct for pages to keep track of     *
    * its future expectation while we look through *
    * the future page references.                  */
   for (page_i = 0; page_i < FRAME_SZ; page_i++) {
      opt[page_i].count = OPT_MAX_LEN;
      opt[page_i].value = frame[page_i];
   }

   /* Loop through pages and put page count until  *
    * one of the pages is found on the future page *
    * references                                   */
   while (*currentPageRefs)
   {
      if (*currentPageRefs == ',') {
         *currentPageRefs++;
         continue;
      }

      num_page_passed++;
      for (frame_i = 0; frame_i < FRAME_SZ; frame_i++)
      {
         /* If page reference found on future pages,    *
          * mark the number of pages until it is found. */
         if (frame[frame_i] == *currentPageRefs) 
         {
            for (page_i = 0; page_i < FRAME_SZ; page_i++)
            {
               if ((opt[page_i].value == frame[frame_i]) && (opt[page_i].count == OPT_MAX_LEN)) 
               {
                  opt[page_i].count = num_page_passed;
                  break;
               }
            }
            break;
         }
      }
      *currentPageRefs++;
   }

   /* Sort by ascending order to define the page ref *
    * with the longest expected reference if any.    */
   qsort(opt, FRAME_SZ, sizeof(PAGE_INFO), asc_sort);

   for (frame_i = 0; frame_i < FRAME_SZ; frame_i++)
   {
      if (frame[frame_i] == opt[0].value) {
         opt_page_i = frame_i;
         break;
      }
   }

   /* Return page index from frame based on expected *
    * longest page reference.                        */
   return opt_page_i;
}

inline int getOptimalPageFaults(char *pageRefs)
{
   int num_pagefault = 0;
   int opt_rep_i = FRAME_SZ - 1;
   int opt_i;
   char opt[FRAME_SZ] = { 0 };

   /* Sets page references into frame array and counts page faults */
   while (*pageRefs)
   {
      if (*pageRefs == ',') {
         *pageRefs++;
         continue;
      }

      /* Loops through the frame array and puts the page into        *
       * a frame if it does not exist already. Every new placement   *
       * signifies a page fault. In the case of a full frame,        *
       * a look ahead occurs and replaces the page that will not be  *
       * used the longest.                                           */
      for (opt_i = 0; opt_i < FRAME_SZ; opt_i++)
      {
         /* If page exist in frame already no page fault occurs.  *
          * Also, its count increases.                            */
         if (opt[opt_i] == *pageRefs) {
            break;
         }

         /* If an empty spot exist in the frame and the page   *
          * does not exist in the frame yet, place it in and   *
          * count a page fault.                                */
         else if (opt[opt_i] == '\0') {
            num_pagefault++;
            opt[opt_i] = *pageRefs;
            break;
         }

         /* If we entered the last frame without finding the   *
          * page it will replace the frame whose page is not   *
          * expected to used for the longest.                  */
         else if ((opt_i + 1) == FRAME_SZ) {
            num_pagefault++;
            /* Obtain index from the frame array with the page ref *
             * that will not be referenced the longest.            */
            opt_rep_i = getOptimalReplacementIndex(opt, pageRefs);
            opt[opt_rep_i] = *pageRefs;
            break;
         }
      }
      *pageRefs++;
   }
   return num_pagefault;
}

int main(int argc, char **argv)
{
   char *pageRefs;
   int pageIndex = 0;
   int pageRef;

   /* Verify the comand line parameters. */
   if (argc != 2) {
      printf("Usage: %s <page references> \n", argv[0]);
      exit(-1);
   }

   pageRefs = argv[1];

   /* Verify valid page refs. */
   while (*(pageRefs + pageIndex) != '\0')
   {
      if (*(pageRefs + pageIndex) == ',') {
         pageIndex++;
         continue;
      }

      /* Check is only digits are present. */
      if (!isdigit(*(pageRefs + pageIndex))) {
         printf("Invalid page reference: %c. Page references must be a number. \n", *(pageRefs + pageIndex));
         exit(-1);
      }

      /* Check that digits are within the minimum and maximum page numbers. */
      pageRef = atoi((const char*)&*(pageRefs + pageIndex));
      if (pageRef < MIN_PAGE_NUM || pageRef > MAX_PAGE_NUM) {
         printf("All page references must be between %d-%d, inclusive. \n", MIN_PAGE_NUM, MAX_PAGE_NUM);
         exit(-1);
      }
      pageIndex++;
   }

   /* Verify number of page refs is not over maximum. */
   if ((pageIndex + 1)/2 >= MAX_PAGE_REFS) {
      printf("A maximum of %d page references is allowed. \n", MAX_PAGE_REFS);
      exit(-1);
   }

   printf("Page Fault Results\n");
   printf("-------------------\n");
   printf("FIFO: %d\n", getFifoPageFaults(pageRefs));
   printf("LRU: %d\n", getLruPageFaults(pageRefs));
   printf("OPTIMAL: %d\n", getOptimalPageFaults(pageRefs));

   return 0;
}
 
