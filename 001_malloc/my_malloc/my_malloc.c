#include<unistd.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "my_malloc.h"
#include<limits.h>

Node_t * head = NULL; //global variable we're gonna use
size_t heap_size = 0;

void * add_new_segment_head(Node_t ** ptHead, size_t size){
  Node_t * first_byte_blk = sbrk(0);
  if(sbrk((sizeof(Node_t) + size)) == (void *) -1){
    return NULL;
  }
  heap_size = heap_size + sizeof(Node_t) + size; //used to test 
  
                //1. allocate new memory,  Let block Num = sizeof((Node_t + size))
  *ptHead = first_byte_blk; //2. next field of the last node of the linked list points at the first byte block of this chunk

  //3. create a Node_t struct starting from the first byte block,set the fields as follows: next = NULL,
  //blk_num = size, isFree = 0
  Node_t temp = {.next = NULL, .prev = NULL, .blk_num = size, .isFree = 0};
  *first_byte_blk = temp;


  /************************NOT A WISE CHOICE********/  
  //   4. advance pointer by (Node_t + size) bytes, let's call it next_blk, create a Node_t struct starting
  //      from next_blk, set blk_num = size, isFree = 1, next = NULL and then let head->next point at
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
  Node_t * first_byte_blk = sbrk(0);
  if(sbrk(size + sizeof(Node_t)) == (void *) -1){
    return NULL;
  }
  heap_size = heap_size + sizeof(Node_t) + size;
  curr->next = first_byte_blk;
  Node_t temp = {.next = NULL, .prev = curr, .blk_num = size, .isFree = 0};
  *first_byte_blk = temp;
  /*
  Node_t *next_blk = (int8_t *) first_byte_blk + size + sizeof(Node_t);
  curr->next->next = next_blk; 
  temp.isFree = 1;
  temp.blk_num = cur_add_size - first_byte_blk->blk_num - 2*sizeof(Node_t);
  temp.prev = curr->next;
  *next_blk = temp;
  */
  return (void *) (first_byte_blk + 1);
}

void * split_and_insert(Node_t * curr, size_t size){
  Node_t * new_meta_info = (Node_t *)( (int8_t *)curr + size + sizeof(Node_t)); //curr address + request bytes + Node_t bytes                                                                 // is actually position of next free block
  Node_t temp = {.next = curr->next, .prev = curr, .blk_num = curr->blk_num - size - sizeof(Node_t), .isFree = 1};
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
  curr->next = new_meta_info;
  curr->blk_num = size;
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
	  if(curr->blk_num == size){
	    curr->isFree = 0;
	    return (void *) (curr + 1);
	  }
	  else if(curr->blk_num >= size + sizeof(Node_t)){
          //split block and return the value
	    curr->isFree = 0;
	    return split_and_insert(curr, size);
	  }
        //if not
	  else if(curr->blk_num >= size && curr->blk_num < size + sizeof(Node_t)){
	    curr->isFree = 0;
	    return (void *) (curr + 1);
	  }
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
      if(curr->blk_num == size){ //this is lieterally the best choice
	*pt_pt_best = curr;
	*pt_best_size = size;
	return;
      }
      else if(curr->blk_num >= size + sizeof(Node_t) && curr->blk_num < *pt_best_size){
	*pt_pt_best = curr;
	*pt_best_size = curr->blk_num;
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
	  curr->isFree = 0;
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
  mergeHead->blk_num = mergeHead->blk_num + sizeof(Node_t) + mergeBody->blk_num;
  mergeHead->next = mergeBody->next;
  if(mergeBody->next != NULL){
    mergeBody->next->prev = mergeHead;
  }
}
      
void ff_free(void * ptr){
  if(ptr != NULL){
    Node_t * temp = (Node_t *) ((int8_t *) ptr - sizeof(Node_t));
    if(temp->isFree){
      fprintf(stderr,"double free for corruption\n");
      exit(EXIT_FAILURE);
    }
    else{
      temp->isFree = 1; //free this block
    //look up to see if prev block is free
      Node_t * prev = temp->prev;
      if(prev == NULL && temp->next == NULL){
	return;
      }
      else{
	if(prev != NULL){
	  if(prev->isFree){
	    merge(prev,temp);
	    if(prev->next != NULL && prev->next->isFree){
	    merge(prev,prev->next);
	    }
	  }
	}
	else{ //prev == NULL but temp->next != NULL
	  if(temp->next->isFree){
	    merge(temp,temp->next);
	  }
	}
      }
    }
  }
}
  
    

void bf_free(void * ptr){
  ff_free(ptr);
}



//performance study auxiliary function

unsigned long get_data_segment_size(){
  return heap_size;
}

unsigned long get_data_segment_free_space_size(){
  Node_t* cur = head;
  unsigned long sum = 0;
  while(cur != NULL){
    if(cur->isFree){
      sum = sum + cur->blk_num;
    }
    cur = cur->next;
  }
  return sum;
}

