#include<unistd.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "my_malloc.h"
#include<limits.h>

void * add_new_segment_head(Node_t ** ptHead, size_t size){
  Node_t * first_byte_blk = brsk(0);
  if(brsk((sizeof(Node_t) + size)) == (void *) -1){
    return NULL;
  } 
                //1. allocate new memory,  Let block Num = sizeof((Node_t + size))
  *ptHead = first_byte_blk; //2. next field of the last node of the linked list points at the first byte block of this chunk

  //3. create a Node_t struct starting from the first byte block,set the fields as follows: next = NULL,
  //blk_size = size, isFree = 0
  Node_t temp = {.next = NULL, .prev = NULL .blk_size = size, .isFree = 0};
  *first_byte_blk = temp;


  /************************NOT A WISE CHOICE********/  
  //   4. advance pointer by (Node_t + size) bytes, let's call it next_blk, create a Node_t struct starting
  //      from next_blk, set blk_size = size, isFree = 1, next = NULL and then let head->next point at
  //      next_blk

  
  /*Node_t *next_blk = (int8_t *) first_byte_blk + size + sizeof(Node_t);
  first_byte_blk->next = next_blk; 
  temp.isFree = 1;
  temp.prev = first_byte_blk;
  *next_blk = temp;
  */ 
  //return the address of the actually avaliable byte block
  return (void *) (first_byte_blk + 1);
}

void * add_new_segment(Node_t * curr, size_t size){
  Node_t * first_byte_blk = brsk(0);
  if(brsk(size + sizeof(Node_t)) == (void *) -1){
    return NULL;
  }
  curr->next = first_byte_blk;
  Node_t temp = {.next = NULL, .prev = curr .blk_size = size, .isFree = 0};
  *first_byte_blk = temp;
  /*
  Node_t *next_blk = (int8_t *) first_byte_blk + size + sizeof(Node_t);
  curr->next->next = next_blk; 
  temp.isFree = 1;
  temp.blk_size = cur_add_size - first_byte_blk->blk_size - 2*sizeof(Node_t);
  temp.prev = curr->next;
  *next_blk = temp;
  */
  return (void *) (first_byte_blk + 1);
}

void * split_and_insert(Node_t * curr, size_t size){
  Node_t * new_meta_info = (int8_t *)curr + size + sizeof(Node_t); //curr address + request bytes + Node_t bytes                                                                 // is actually position of next free block             
  Node_t temp = {.next = curr->next, .prev = curr .blk_size = curr->blk_size - sizeof(Node_t), .isFree = 1};
  //this new free block group will point at the block group its ancestor pointing at
  //its previous pointer will point at its ancestor
  //the avaliable byte block number is equal to current blk number - Node size(meta info)
  //and it is free
  *new_meta_info = temp;
  if(curr->next != NULL){ //we have next block group
    new_meta_info->next->prev = new_meta_info;
      //modify prev field in next block pointing at
      //new_meta_info
  }
  return (void *) (curr + 1);
}
void * ff_malloc(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    if( head == NULL){
      return add_new_segment_head(&head,size);
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
	  if(curr->blk_size == size){
	    curr->isFree == 0;
	    return (void *) (curr + 1);
	  }
	  else if(curr->blk_size >= size + sizeof(Node_t)){
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
      return add_new_segment(prev, size);
    }
  }
}


void find_best_position(size_t size, Node_t ** pt_prev, Node_t ** pt_pt_best, size_t * pt_best_size){
  Node_t * curr = head;
  while(curr != NULL){
    if(curr->isFree){
      if(curr->blk_size == size){ //this is lieterally the best choice
	*pt_pt_best = curr;
	*pt_best_size = size;
	return;
      }
      else if(curr->blk_size >= size + sizeof(Node_t) && curr->blk_size < *pt_best_size){
	*pt_pt_best = curr;
	*pt_best_size = curr->blk_size;
      }
    }
    *pt_prev = curr;
    curr = curr->next;
  }					      
}
void * bf_malloc(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    if( head == NULL){
      return add_new_segment_head(&head,size);
    }
    else{ // this is not the first malloc call
      Node_t * curr = head;
      Node_t * prev = NULL;
      Node_t * pt_cur_best = NULL;
      size_t cur_best_size = UINT_MAX;
      find_best_position(size, &prev, &pt_cur_best, &cur_best_size);
      if(pt_cur_best != NULL){
	if(cur_best_size == size){ //ideal fit
	  curr->isFree == 0;
	  return (void *) (curr + 1);
	}
	else{
	  return split_and_insert(pt_cur_best, size);
	}
      }
      else{
	return add_new_segment(prev,size);
      }
    }
  }
}











  
void merge(Node_t * mergeHead, Node_t * mergeBody){
  mergeHead->blk_size = mergeHead + sizeof(Node_t) + mergeBody->blk_size;
  mergeHead->next = mergeBody->next;
  if(mergeBody->next != NULL){
    mergeBody->next->prev = mergeHead;
  }
}
      
void ff_free(void * ptr){
  if(ptr != NULL){
    Node_t * temp = (int8_t *) ptr - sizeof(Node_t);
    temp->isFree = 1; //free this block
    //look up to see if prev block is free
    Node_t * prev = temp->prev;
    if(prev->isFree){
      merge(prev,temp);
    }
    //merge two blocks
    //look up to see if next lock is free
    if(prev->next != NULL && prev->next->isFree){
    //if it is free
    //merge two blocks
      merge(prev,prev->next);
    }
  }
}

void bf_free(void * ptr){
  ff_free(ptr);
}
