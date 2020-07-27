#include <stdio.h>
#include <assert.h>
#include <math.h>// for floor and log2
#include "buddy_allocator.h"

#define KYEL  "\x1B[33m"
#define KMAG  "\x1B[35m"
#define KWHT  "\x1B[37m"
#define KRED  "\x1B[31m"

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

// computes the size in bytes for the buffer of the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int num_bits=1<<(num_levels+1);
  if(num_bits%8 == 0) {
  	return num_bits/8;
  }
  else{
  	return (num_bits/8)+1;
  }
}


//cg initializes the buddy allocator
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         uint8_t* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size){

  // initializition of parameters
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  //checking for a correct nrum_levels
  assert (num_levels<MAX_LEVELS);
  // we need enough memory to handle internal structures
  assert (buffer_size>=BuddyAllocator_calcSize(num_levels));   

  printf("%s", KYEL);
  printf("BUDDY INITIALIZING\n");
  printf("%s", KWHT);
  printf("\tlevels: %d\n", num_levels);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
  
  //cg: inizialization bitmap
  int num_bits=(1<<(num_levels+1));
  BitMap_init(&(alloc->bit_map), num_bits, buffer);

  //cg: setting to 1 each bit
  for(int i=1; i<num_bits; i++){
    BitMap_setBit(&(alloc->bit_map), i, 1);     
  }

  
 //STAMPA BITMAP
   printf("%s", KMAG);  
   printf("\nSTAMPA BITMAP");
   printf("%s", KWHT);  
   BuddyAllocator *alloc_pointer=alloc;
   //BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
   //int max=bit_map_pointer->num_bits;
   for (int b=1; b<=255; b++){
   	if(b==1 || b==2 || b==4 || b==8 || b==16 || b==32 || b==64 || b==128 ) {
   		printf("\n");
		for(int s=0; s<(70-b)+b/2; s++) printf(" ");
   	}
    printf("%d",BitMap_bit(&(alloc_pointer->bit_map) , b));
   }
   printf("\n");


};


//mc: recursive fucntion to set to 0 ancestors of a node
void set_antenati(BitMap bit_map,int node){
	//mc: root node
	if (node==1) {
		return;
	}
	int padre=parentIdx(node);
	//mc: if the father is already set to zero stop
	if (BitMap_bit(&bit_map,padre)==0) return;
	printf("--->setting to zero the node: %d, level: %d\n",padre,levelIdx(padre));  		
	//mc: setting to 0 the father node and its ancestors
	BitMap_setBit(&bit_map, padre , 0);
	set_antenati(bit_map,padre);
}

//mc :recursive fucntion to set to 0 ancestors of a node
void set_successori(BitMap bit_map,int node){
	BitMap *bit_map_pointer=&bit_map;
	//mc: the tree finished
	if (node>=bit_map_pointer->num_bits) {
		return;
	}
	//mc: compute successors
	int successore_sx=node*2;
	int successore_dx=(node*2)+1;
    //mc: setting to 0 successors
	BitMap_setBit(&bit_map, successore_sx , 0);	
	BitMap_setBit(&bit_map, successore_dx , 0);
	set_successori(bit_map,successore_sx);
	set_successori(bit_map,successore_dx);
}


//mc: return a free node in the requested level of the tree
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  if (level<0)
    return 0;
  //mc: the level is correct
  assert(level <= alloc->num_levels);

  //mc: computing the start and end node in that level
  int start = 1<<level;
  int end= (1<<(level+1))-1;

  //mc :loop to search a free node in the level
  for (int i=start; i<=end; i++){
    //mc: if a node is free we set to 0 that node and its successors and ancestors
  	if(BitMap_bit(&(alloc->bit_map), i)==1){ 
  		BitMap_setBit(&(alloc->bit_map), i, 0);
  		set_antenati(alloc->bit_map,i);
  		set_successori(alloc->bit_map,i);
  		return i;
  	}
  }
  return -1;
}

//cg :recursive fucntion to set to 1 successors of a node
void set_successori_uno(BitMap bit_map,int node){
  BitMap *bit_map_pointer=&bit_map;
  //cg: if the tree is finisched we stop
  if (node>=bit_map_pointer->num_bits) {
    return;
  }
  //cg: we compute children
  int successore_sx=node*2;
  int successore_dx=(node*2)+1;
  //setting to 1 successors
  BitMap_setBit(&bit_map, successore_sx , 1);
  BitMap_setBit(&bit_map, successore_dx , 1);
  set_successori_uno(bit_map,successore_sx);
  set_successori_uno(bit_map,successore_dx);
}

//cg: recursive fucntion to set to 1 ancestors of a node
void set_padri_uno( BitMap bit_map, int node){
  int bro=buddyIdx(node);
  int padre=parentIdx(node);

  //cg: if the brother is allocated, the father can't be free
  if(BitMap_bit(&bit_map, bro)==0) return;
  //cg: setting to 1 che father node and ancestors
  BitMap_setBit(&bit_map, padre , 1);  
  printf("--->setting to one the node: %d, level: %d \n",padre,levelIdx(padre));  		
  set_padri_uno(bit_map,padre);
 

}

//cg: it sets to 1 the requested node in the buffer
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int node){
  if(node<0) return;
  
  BitMap *bit_map_pointer = &(alloc->bit_map);
  int tot_bit = bit_map_pointer->num_bits;
  assert(node <= tot_bit);

  //cg: if the bit is already set to 1 , it' already free 
  if(BitMap_bit(&(alloc->bit_map), node)==1){
  	printf("%s", KRED);
    printf("\nERROR: ");
    printf("%s", KWHT);
    printf("node already deallocated\n");
    return;
  }

  //cg: setting to 1 the current node and successors and ancestors
  BitMap_setBit(&(alloc->bit_map), node, 1);  

  printf("%s", KMAG);  
  printf("\nsetting requested node:\n");
  printf("%s", KWHT);
  printf("--->setting to one the node: %d, level: %d \n",node,levelIdx(node));  		
  set_successori_uno(alloc->bit_map, node);
  printf("%s", KMAG);
  printf("\nsetting predecessors:\n");
  printf("%s", KWHT);
  set_padri_uno(alloc->bit_map ,node);
 
}

//mc: it returns the number of a free node where allocate memory
void *BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {

  if (size>1048568){
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  	printf("%s", KRED);
  	printf("\nERROR: ");
  	printf("%s", KWHT);
    printf("%s", KYEL);
    printf("the requested memory is over the limit\n");
    printf("%s", KWHT);
    return 0;
  }

  //mc: we determine the level of the page
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  int  level=floor(log2(mem_size/(size+8)));


  //mc:  if the level is too small, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("%s", KYEL);
  printf("\nrequested: %d bytes, level %d \n",size, level);
  printf("%s", KWHT);

  printf("%s", KMAG);
  printf("\nsetting predecessors:\n");
  printf("%s", KWHT);
  
  //mc: we get the node calling auxiliary function
  int node=BuddyAllocator_getBuddy(alloc, level);
  //mc: if returns -1 there aren't more blocks for that size
  if (node==-1){
   	printf("%s", KRED); 	
    printf("\nERROR: ");
    printf("%s", KWHT);
    printf("no more free blocks for the requested size\n\n");
    return 0;
  }

  //mc: the correct size of the block
  int buddy_size=mem_size>>level;

  int offset=node%(1<<level);

  //mc: we write at the beginning of the memory the number of the node
  printf("%s", KMAG);
  printf("\nsetting requested node:\n");
  printf("%s", KWHT);
  int *p =(int*)(alloc->memory+(offset*buddy_size));
  *p = node;
  printf("--->setting to zero the node: %d, level: %d, start: %p, size: %d\n",node,level, p+4, size);  		



 //STAMPA BITMAP
   printf("%s", KMAG);  
   printf("\nSTAMPA BITMAP");
   printf("%s", KWHT);  
   BuddyAllocator *alloc_pointer=alloc;
   //BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
   //int max=bit_map_pointer->num_bits;
   for (int b=1; b<=255; b++){
   	if(b==1 || b==2 || b==4 || b==8 || b==16 || b==32 || b==64 || b==128 ) {
   		printf("\n");
		for(int s=0; s<(70-b)+b/2; s++) printf(" ");
   	}
    printf("%d",BitMap_bit(&(alloc_pointer->bit_map) , b));
   }
   printf("\n");


  //mc: we return the memory
  return p+4; 
  
}


//mc: it deallocates a memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {

  //mc: if it wasn't allocate there is an error
  if (mem==0) {
   	printf("%s", KRED); 	
    printf("\nERROR:  ");
    printf("%s", KWHT);
    printf("this memory wasn't allocated, so it's impossible to free\n");

    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    return;
  }
  printf("%s", KYEL);
  printf("\nfreeing: %p \n",mem);
  printf("%s", KWHT);  

  //mc: we get the node
  int *p=(int*)mem;
  p=p-4;
  printf("\nnode: %d\n", *p);

  //mc: we call an auxiliary function
  BuddyAllocator_releaseBuddy(alloc, *p);

 //STAMPA BITMAP
   printf("%s", KMAG);  
   printf("\nSTAMPA BITMAP");
   printf("%s", KWHT);  
   BuddyAllocator *alloc_pointer=alloc;
   //BitMap* bit_map_pointer = &(alloc_pointer->bit_map);
   //int max=bit_map_pointer->num_bits;
   for (int b=1; b<=255; b++){
   	if(b==1 || b==2 || b==4 || b==8 || b==16 || b==32 || b==64 || b==128 ) {
   		printf("\n");
		for(int s=0; s<(70-b)+b/2; s++) printf(" ");
   	}
    printf("%d",BitMap_bit(&(alloc_pointer->bit_map) , b));
   }
   printf("\n");

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");


  
}
