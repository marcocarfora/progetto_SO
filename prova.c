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
/*
  BuddyAllocator *alloc_pointer=&alloc;
  BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
  int max=bit_map_pointer->num_bits;
   for (int j=0; j<=max; j++){
	printf("%d",BitMap_bit(bit_map_pointer , j));
  }
  printf("STOOOOP\n");
  BitMap_setBit(bit_map_pointer, 135, 0);

  for (int j=0; j<=max; j++){
	printf("%d",BitMap_bit(bit_map_pointer , j));
  }
*/

  int res1=BuddyAllocator_getBuddy(&alloc, 10);
  printf("res1: %d\n", res1);
  int res2=BuddyAllocator_getBuddy(&alloc, 2);
  printf("res2: %d\n", res2);
  int res3=BuddyAllocator_getBuddy(&alloc, 1);
  printf("res3: %d\n", res3);


 	return 1;  
    
}



