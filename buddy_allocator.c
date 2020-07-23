#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"
//#include "bit_map.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
//  return (int)floor(log2(idx));
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}





void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         uint8_t* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size){

  // we need room also for level 0
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  assert (num_levels<MAX_LEVELS);
  // we need enough memory to handle internal structures
 // assert (buffer_size>=BuddyAllocator_calcSize(num_levels));   ///??????????????????????????????????????????

//  int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
//  int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d", num_levels);
//  printf("\tmax list entries %d bytes\n", list_alloc_size);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
 

  int num_bits=(1<<(num_levels+1));
  BitMap_init(&(alloc->bit_map), num_bits, buffer_size, buffer);

  for(int i=0; i<=num_bits; i++){
    BitMap_setBit(&(alloc->bit_map), i, 1);    
  }

};
