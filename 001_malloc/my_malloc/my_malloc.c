#include<unistd.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "my_malloc.h"
#include<limits.h>
#include <assert.h>
Node_t * tail = NULL;
Node_t *  free_head = NULL;
Node_t * head = NULL; //global variable we're gonna use
size_t heap_size = 0;
/*void print_Node_t(Node_t * ptNode_t){
  printf("***************printing all nodes********************\n");
  Node_t* cur = ptNode_t;
  while(cur != NULL){
    printf("============================================\n");
    printf("address of cur Node_t: %p\n",(void *)cur);
    printf("address of next_free Node_t: %p\n",(void *)cur->next);
    printf("address of prev_free Node_t: %p\n",(void *)cur->prev);
    printf("avalible blocks number for this region:%lu\n",cur->blk_num);
    printf("address of next conseutive block: %p\n",(void *)cur->log_next);
    printf("address of prev consecutive block: %p\n",(void *)cur->log_prev);
    printf("address of its next adjacent region: %p\n",(void *)((int8_t *)(cur + 1) + cur->blk_num));
    cur = cur->log_next;
  }
}
void print_free_list(Node_t * ptNode_t){
  printf("***************printing free list********************\n");
  Node_t * cur = ptNode_t;
  while(cur != NULL){
    printf("address of cur Node_t: %p\n",(void *)cur);
    printf("address of next_free Node_t: %p\n",(void *)cur->next);
    printf("address of prev_free Node_t: %p\n",(void *)cur->prev);
    cur = cur->next;
  }
}
*/
void * add_new_segment_head(size_t size){
  Node_t * first_byte_blk = sbrk(0);
  if(sbrk((sizeof(Node_t) + size)) == (void *) -1){
    return NULL;
  }
  heap_size = heap_size + sizeof(Node_t) + size; //used to test 
  
                //1. allocate new memory,  Let block Num = sizeof((Node_t + size))
  head = first_byte_blk;
  tail = head;//2. next field of the last node of the linked list points at the first byte block of this chunk

  //3. create a Node_t struct starting from the first byte block,set the fields as follows: next = NULL,
  //blk_num = size, isFree = 0
  Node_t temp = {.next = NULL, .prev = NULL, .blk_num = size, .log_prev = NULL, .log_next = NULL};
  *first_byte_blk = temp;
  //debugging
  //print_Node_t(head);        
  //print_free_list(free_head);
  return (void *) (first_byte_blk + 1);
}

void * add_new_segment(Node_t * curr, size_t size){
  Node_t * first_byte_blk = sbrk(0);
  if(sbrk(size + sizeof(Node_t)) == (void *) -1){
    return NULL;
  }
  heap_size = heap_size + sizeof(Node_t) + size;
  Node_t temp = {.next = NULL, .prev = NULL, .blk_num = size, .log_prev = curr, .log_next = NULL};
  curr->log_next = first_byte_blk;
  tail = first_byte_blk; //change tail
  *first_byte_blk = temp;
  //debugging
  //print_Node_t(head);        
  //print_free_list(free_head);
  return (void *) (first_byte_blk + 1);
}

void * split_and_insert(Node_t * curr, size_t size){
  Node_t * new_meta_info = (Node_t *)( (int8_t *)curr + size + sizeof(Node_t)); //curr address + request bytes + Node_t bytes                                                                 // is actually position of next free block
  Node_t temp = {.next = NULL, .prev = NULL, .blk_num = curr->blk_num - size - sizeof(Node_t), .log_prev = curr, .log_next = curr->log_next};
  *new_meta_info = temp;
  deleteNode(curr);
  addToFreeList(new_meta_info);
  curr->log_next = new_meta_info;
  if(new_meta_info->log_next != NULL){
    new_meta_info->log_next->log_prev = new_meta_info; //next logic node's previous node
  }
  else{
    tail = new_meta_info; //change tail
  }
  //if(new_meta_info->prev == NULL && new_meta_info->next == NULL){ 
  curr->blk_num = size;
  //debugging
  //print_Node_t(head);
  //print_free_list(free_head);
  return (void *) (curr + 1);
}


void deleteNode(Node_t * toRemove){
  if(free_head == toRemove){
    free_head = toRemove->next;
  }
  if(toRemove->next != NULL){
      toRemove->next->prev = toRemove->prev;
  }
  if(toRemove->prev != NULL){
    toRemove->prev->next = toRemove->next;
  }
  toRemove->next = NULL;
  toRemove->prev = NULL;
}


void * ff_malloc(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    if( head == NULL){
      return add_new_segment_head(size);
    }
    else{ // this is not the first malloc call
      Node_t * curr = free_head;
      while(curr != NULL){
	  if(curr->blk_num >= size &&
	     curr->blk_num < size + sizeof(Node_t)){
	    //delete this node from the free list
	    deleteNode(curr);
	    return (void *) (curr + 1);
	  }  
	  else if(curr->blk_num >= size + sizeof(Node_t)){
          //split block and return the value
	    return split_and_insert(curr, size);
	  }
        //if not
	  else{
	    //go to the next free block
	    curr = curr->next;
	    continue;
	  }
      }
      //add_new_segment
      return add_new_segment(tail,size);
    }
  }
}


void find_best_position(size_t size, Node_t ** pt_pt_best, size_t * pt_best_size){
  Node_t * curr = free_head;
  while(curr != NULL){
    if(curr->blk_num == size){ //this is lieterally the best choice
      *pt_pt_best = curr;
      *pt_best_size = size;
      return;
    }
    else if(curr->blk_num > size && curr->blk_num < *pt_best_size){
      *pt_pt_best = curr;
      *pt_best_size = curr->blk_num;
    }
    curr = curr->next;
  }					      
}
void * bf_malloc(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    if( head == NULL){
      return add_new_segment_head(size);
    }
    else{ // this is not the first malloc call
      //Node_t * curr = head;
      Node_t * pt_cur_best = NULL;
      size_t cur_best_size = -1;
      find_best_position(size,&pt_cur_best, &cur_best_size);
      if(pt_cur_best != NULL){
	if(cur_best_size >= size && cur_best_size < size + sizeof(Node_t)){ //no split fit
	  deleteNode(pt_cur_best);
	  return (void *) (pt_cur_best + 1);
	}
	else{
	  return split_and_insert(pt_cur_best, size);
	}
      }
      else{
	return add_new_segment(tail,size);
      }
    }
  }
}










  
void merge_first_part(Node_t * mergeHead){
  mergeHead->blk_num = mergeHead->blk_num + sizeof(Node_t) + mergeHead->log_next->blk_num;
  mergeHead->log_next = mergeHead->log_next->log_next;
  if(mergeHead->log_next != NULL){
    mergeHead->log_next->log_prev = mergeHead;
  }
  else{
    tail = mergeHead;
  }
}
void merge_second_part(Node_t * mergeHead){
  //the log_next is in the free list
  //We first deal with pointers pointing at consecutive blocks
  Node_t * temp = mergeHead->log_next;
  merge_first_part(mergeHead);
  deleteNode(temp);
}
  

/*int isInsideFreeList(Node_t * toCheck){
  Node_t * curr = free_head;
  while ( curr != NULL){
    if (curr == toCheck){
      return 1;
    }
    curr = curr->next;
  }
  return 0;
}
*/

void addToFreeList(Node_t * toAdd){
  toAdd->next = free_head;
  if(free_head != NULL){
    free_head->prev = toAdd;
  }
  free_head = toAdd;
}

void ff_free(void * ptr){
  if(ptr != NULL){
    Node_t * toCheck = (Node_t *) ptr - 1;
    //assert(!(toCheck->prev != NULL || toCheck->next != NULL || toCheck == free_head));
    if(toCheck > (Node_t *)sbrk(0) || toCheck < head){
      fprintf(stderr,"Segmentation Fault\n");
      exit(EXIT_FAILURE);
    }
    if(toCheck->prev != NULL || toCheck->next != NULL || toCheck == free_head){
      fprintf(stderr,"double free for corruption\n");
      exit(EXIT_FAILURE);
    }
    //else{
    //look up to see if toCheck is head, if it is head, we don't need to check prev node
    if(toCheck != head &&
       (toCheck->log_prev->next != NULL ||
	toCheck->log_prev->prev != NULL ||
	toCheck->log_prev == free_head)){
      Node_t * temp = toCheck->log_prev;
      merge_first_part(toCheck->log_prev);
      if(temp != tail &&
	 (temp->log_next->next != NULL ||
	  temp->log_next->prev != NULL ||
	  temp->log_next == free_head)){
	merge_second_part(temp);
      }
    }
    else if(toCheck != tail &&
	    (toCheck->log_next->next != NULL ||
	     toCheck->log_next->prev != NULL ||
	     toCheck->log_next == free_head)){
      addToFreeList(toCheck);    
      merge_second_part(toCheck);
    }
    else{
      addToFreeList(toCheck);
    }
    //print_Node_t(head);        
    //print_free_list(free_head);
  }
  //}
}

    

void bf_free(void * ptr){
  ff_free(ptr);
}



//performance study auxiliary function

unsigned long get_data_segment_size(){
  return heap_size;
}

unsigned long get_data_segment_free_space_size(){
  Node_t* cur = free_head;
  unsigned long sum = 0;
  while(cur != NULL){
    sum = sum + cur->blk_num /*+ sizeof(Node_t)*/;
    cur = cur->next;
  }
  return sum;
}
