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

  //printf("%ld\n", sizeof(memory));


  BuddyAllocator alloc;
  BuddyAllocator_init(&alloc,BUDDY_LEVELS,buffer,BUFFER_SIZE,memory,MIN_BUCKET_SIZE);

/*  BuddyAllocator *alloc_pointer=&alloc;
  BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
  int max=bit_map_pointer->num_bits;
  int res1=BuddyAllocator_getBuddy(&alloc, 2);
  int res2=BuddyAllocator_getBuddy(&alloc, 3);

  //stampa totale
  printf("res1: %d\n", res1);
  printf("res2: %d\n", res2);
   for (int j=0; j<=max; j++){
	  printf("%d",BitMap_bit(bit_map_pointer , j));
  }
  printf("\nfine\n");

  BuddyAllocator_releaseBuddy(&alloc,res1);
  BuddyAllocator_releaseBuddy(&alloc,res2);

  printf("\npausa\n");
  for (int j=0; j<=max; j++){
    printf("%d",BitMap_bit(bit_map_pointer , j));
  }
*/
  void* p1=BuddyAllocator_malloc(&alloc, 100);
  void* p2=BuddyAllocator_malloc(&alloc, 100);
  void* p3=BuddyAllocator_malloc(&alloc, 100000);
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3);
  

 	return 1;  
    
}



