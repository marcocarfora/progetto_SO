#pragma once
#include "bit_map.h"
#include <stdint.h>




#define MAX_LEVELS 16

typedef struct  {
  BitMap bit_map;
  int num_levels;
  char* memory; // the memory area to be managed
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;


//mc: recursive fucntion to set to 0 ancestors of a node
void set_antenati(BitMap bit_map,int node);
//mc :recursive fucntion to set to 0 ancestors of a node
void set_successori(BitMap bit_map,int node);

// computes the size in bytes for the buffer of the allocator
int BuddyAllocator_calcSize(int num_levels);

// cg: initializes the buddy allocator
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         uint8_t* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size);

//mc: return a free node in the requested level of the tree
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);

//cg: it sets to 1 the requested node in the buffer
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int node);

//mc: it returns the number of a free node where allocate memory
void *BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

//mc: it deallocates a memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

