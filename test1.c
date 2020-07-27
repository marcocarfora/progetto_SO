#include <stdio.h>
#include <assert.h>
#include "buddy_allocator.h"
//#include "bit_map.h"
#define BUFFER_SIZE 102400
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))

uint8_t buffer[BUFFER_SIZE]; // 100 Kb buffer to handle memory should be enough
char memory[MEMORY_SIZE];

int main(){
  

  BuddyAllocator alloc;
  BuddyAllocator_init(&alloc,BUDDY_LEVELS,buffer,BUFFER_SIZE,memory,MIN_BUCKET_SIZE);
  printf("***end initialization***\n");


  void* p1=BuddyAllocator_malloc(&alloc, 100);
  void* p2=BuddyAllocator_malloc(&alloc, 100);
  void* p3=BuddyAllocator_malloc(&alloc, 100000);
  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3);
  

 	return 1;  
    
}



