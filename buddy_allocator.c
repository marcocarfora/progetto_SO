#include <stdio.h>
#include <assert.h>
#include <math.h>// for floor and log2
#include "buddy_allocator.h"
//#include "bit_map.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
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

//int startIdx(int idx){
//  return (idx-(1<<levelIdx(idx)));
//}





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

  for(int i=1; i<=num_bits; i++){
    BitMap_setBit(&(alloc->bit_map), i, 1);     
  }

};


//funzione rzicorsiva per settare a 0 tutti gli antenati di un nodo
void set_antenati(BitMap bit_map,int node){
	if (node==1) {
		return;
	}
	int padre=parentIdx(node);
	BitMap_setBit(&bit_map, padre , 0);
	set_antenati(bit_map,padre);
}

void set_successori(BitMap bit_map,int node){
	BitMap *bit_map_pointer=&bit_map;
	if (node>=bit_map_pointer->num_bits) {
		return;
	}
	int successore_sx=node*2;
	int successore_dx=(node*2)+1;
	BitMap_setBit(&bit_map, successore_sx , 0);
	BitMap_setBit(&bit_map, successore_dx , 0);
	set_successori(bit_map,successore_sx);
	set_successori(bit_map,successore_dx);
}



int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  if (level<0)
    return 0;
  assert(level <= alloc->num_levels);

  int start = 1<<level;
  int end= (1<<(level+1))-1;


  for (int i=start; i<=end; i++){
  	if(BitMap_bit(&(alloc->bit_map), i)==1){
  		BitMap_setBit(&(alloc->bit_map), i, 0);
  		set_antenati(alloc->bit_map,i);
  		set_successori(alloc->bit_map,i);
  		return i;
  	}
  }
  return -1;
}


void set_successori_uno(BitMap bit_map,int node){
  BitMap *bit_map_pointer=&bit_map;
  if (node>=bit_map_pointer->num_bits) {
    return;
  }
  int successore_sx=node*2;
  int successore_dx=(node*2)+1;
  BitMap_setBit(&bit_map, successore_sx , 1);
  BitMap_setBit(&bit_map, successore_dx , 1);
  set_successori_uno(bit_map,successore_sx);
  set_successori_uno(bit_map,successore_dx);
}


void set_padri_uno( BitMap bit_map, int node){
  int bro=buddyIdx(node);
  int padre=parentIdx(node);

  if(BitMap_bit(&bit_map, bro)==0) return;
  BitMap_setBit(&bit_map, padre , 1);  
  set_padri_uno(bit_map,padre);
 

}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int node){
  if(node<0) return;

  BitMap *bit_map_pointer = &(alloc->bit_map);
  int tot_bit = bit_map_pointer->num_bits;
  assert(node <= tot_bit);

  assert(BitMap_bit(&(alloc->bit_map), node)==0);

  BitMap_setBit(&(alloc->bit_map), node, 1);  
  set_successori_uno(alloc->bit_map, node);
  set_padri_uno(alloc->bit_map ,node);
 
}



void *BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {

  // we determine the level of the page
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  int  level=floor(log2(mem_size/(size+8)));


  // if the level is too small, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;

  printf("requested: %d bytes, level %d \n",size, level);

  int node=BuddyAllocator_getBuddy(alloc, level);

  //////////////////////////////

  //STAMPA BITMAP
   BuddyAllocator *alloc_pointer=alloc;
   BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
   int max=bit_map_pointer->num_bits;
   for (int j=1; j<=max; j++){
    printf("%d",BitMap_bit(&(alloc_pointer->bit_map) , j));
   }


  /////////////////////////////////////

  if (! node) return 0;	
  printf("node: %d\n ",node);  

  int buddy_size=mem_size>>level;
  printf("buddy_size: %d\n ",buddy_size);   
  printf("alloc->min_bucket_size: %d\n ",alloc->min_bucket_size);
  //int buddy_size_giorgio=alloc->min_bucket_size;
  //printf("buddy_size_giorgio: %d\n ",buddy_size_giorgio);   
  int offset=node%(1<<level);
  printf("offset: %d\n ",offset);
  //int offset2=((node-(1<<levelIdx(node))) << (alloc->num_levels-level));
  //printf("offset2: %d\n ",offset2);
  int *pointer=(int *)alloc->memory+(offset*buddy_size);
  printf("pointer: %p\n ",pointer);
  //void *pointer2 = alloc->memory + ((node-(1<<levelIdx(node))) << (alloc->num_levels-level) )*alloc->min_bucket_size;
  //printf("pointer2: %p\n ",pointer2);
  *pointer = node;

  return pointer+4; //poichè la malloc ritorna la quantità allocata

  
}


void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  
  int* p=(int*) mem;
  p=p-4;
  BuddyAllocator_releaseBuddy(alloc, *p);

  /////////////////////////////////////

  //STAMPA BITMAP
   BuddyAllocator *alloc_pointer=alloc;
   BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
   int max=bit_map_pointer->num_bits;
   for (int j=1; j<=max; j++){
    printf("%d",BitMap_bit(&(alloc_pointer->bit_map) , j));
   }
   printf("\n....................");

   /////////////////////////////////////

  
}
