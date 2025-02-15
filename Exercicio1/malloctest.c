
//Para compilar:
// gcc -o malloctest malloctest.c memoryallocator.c

#include "memoryallocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/* Maximum number of memory blocks that can be allocated */
#define MAX_ALLOCATIONS 1000  

struct allocation_t{
    void *ptr;
    unsigned int size;
} allocations[MAX_ALLOCATIONS];

int main(int argc, char *argv[]) {
    unsigned int currentMemory = 0;
    int allocCount = 0;
    unsigned int maxMemory;  
    unsigned int size;

    if (argc < 2){
        printf("Usage: %s <max memory in bytes>\n", argv[0]);
        return 1;
    }

    maxMemory = atoi(argv[1]);

    srand(time(NULL));

    init_memory(maxMemory);

    while(1){
        if (rand() % 2 == 0 && allocCount < MAX_ALLOCATIONS - 1){
            /* Allocate memory */
            size = rand() % 100;
            if (currentMemory + size > maxMemory){
                printf("Out of memory\n");
                continue;
            }
            /* Change malloc() by smalloc() */
            allocations[allocCount].ptr = smalloc(size);
            //printf("tamanho do bloc: %d\n",allocations[allocCount].size);
            allocations[allocCount].size = size;
            //printf("tamanho do bloc: %d\n",allocations[allocCount].size);
            
            currentMemory += size;
            allocCount++;
            printf("Allocated %u bytes, total %u, position %i\n", size, currentMemory, allocCount);
        }else{
            /* Free memory */
            if (allocCount > 0){
                allocCount--;
                size = allocations[allocCount].size;
                currentMemory -= size;
                /* Change free() by sfree() */
                sfree(allocations[allocCount].ptr);
                printf("Free %u bytes, total %u, position %i\n", size, currentMemory, allocCount);
            }
        }
    }

    return 0;
}




