#include<unistd.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "my_malloc.h"


void * add_new_segment(Node_t ** end, size_t size){
  void * first_byte_blk = brsk(2*(sizeof(Node_t) + size));
                //1. allocate new memory, to amortize syscall time, Let block Num = sizeof(2*(Node_t + size)
  *end = first_byte_blk; //2. next field of the last node of the linked list points at the first byte block of
                       //this chunk

  //3. create a Node_t struct starting from the first byte block,set the fields as follows: next = NULL,
  //blk_size = size, isFree = 0
  Node_t temp = {.next = NULL, .blk_size = size, .isFree = 0};
  *(Node_t *)first_byte_blk = temp;

  
  //   4. advance pointer by (Node_t + size) bytes, let's call it next_blk, create a Node_t struct starting
  //      from next_blk, set blk_size = size, isFree = 1, next = NULL and then let head->next point at
  //      next_blk
  Node_t *next_blk = (int8_t *) first_byte_blk + size + sizeof(Node_t);
  (*(Node_t *)first_byte_blk).next = next_blk; 
  temp.isFree = 1;
  *(Node_t *)next_blk = temp;


  //return the address of the actually avaliable byte block
  return (Node_t *) first_byte_blk + 1;
}

void * split_and_insert(Node_t * curr, size_t size){
  void * new_meta_info = (int8_t *)curr + size + sizeof(Node_t); //curr address + request bytes + Node_t bytes                                                                 // is actually position of next free block             
  Node_t temp = {.next = curr->next,.blk_size = curr->blk_size - sizeof(Node_t), .isFree = 1};
  //this new free block group will point at the block group its ancestor pointing at
  //the avaliable byte block number is equal to current blk number - Node size(meta info)
  //and it is free
  *((Node_t *) new_meta_info) = temp;
  curr->next = (Node_t *) new_meta_info;
  return (Node_t *) curr + 1;
}
void * ff_malloc(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    if( head == NULL){
      return add_new_segment(&head,size);
    }
    else{ // this is not the first malloc call
      Node_t * curr = head;
      Node_t * prev = NULL;
      //we iterate starting from the head and check if each block group is avaliable
      while(curr != NULL){
      //if free = 0, that means it is occupied, go to the next block
	if(!curr->isFree){
	  prev = curr;
	  curr = curr->next;
	  continue;
	}
	else{
      //if free != 0
        //if has enough space
	  if(curr->blk_size >= size + sizeof(Node_t)){
          //split block and return the value
	    return split_and_insert(curr, size);
	  }
        //if not
	  else{
          //go to the next block
	    prev = curr;
	    curr = curr->next;
	    continue;
	  }
	}
      }
      //add_new_segment
      return add_new_segment(&prev->next, size);
    }
  }
}
      
      
      
void free(void * ptr){
  if(ptr != NULL){
    //free this block
    //start from the head and defragment adjacent free blocks
  }
}
