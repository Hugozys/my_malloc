#include<unistd.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "my_malloc.h"
#include<limits.h>
#include<pthread.h>
#include <assert.h>
Node_t * tail = NULL;
Node_t * free_head = NULL;
Node_t * head = NULL; //global variable we're gonna use
size_t heap_size = 0;

__thread Node_t * free_head_nolock = NULL; //this is used for lock free malloc

//size of meta information
const size_t metaSize = sizeof(Node_t); 

pthread_mutex_t sbrk_lock = PTHREAD_MUTEX_INITIALIZER;

void * ts_malloc_lock(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    void * ans;
    pthread_mutex_lock(&sbrk_lock);
    if( head == NULL){
      ans = add_new_segment_head(size);
    }
    else{ // this is not the first malloc call
      Node_t * pt_cur_best = NULL;
      size_t cur_best_size = -1;
      find_best_position(size,&pt_cur_best, &cur_best_size);
      if(pt_cur_best != NULL){
	if(cur_best_size >= size && cur_best_size < size + metaSize){ //no split fit
	  deleteNode(pt_cur_best);
	  ans = (void *) (pt_cur_best + 1);
	}
	else{
	  ans = split_and_insert(pt_cur_best, size);
	}
      }
      else{
	ans = add_new_segment(tail,size);
      }
    }
    pthread_mutex_unlock(&sbrk_lock);
    return ans;
  }
}



void ts_free_lock(void * ptr){
  if(ptr != NULL){
    pthread_mutex_lock(&sbrk_lock);
    Node_t * toCheck = (Node_t *) ptr - 1;
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
    pthread_mutex_unlock(&sbrk_lock);
  }
}






void * add_new_segment_head(size_t size){
  //no other thread can increment the heap
  Node_t * first_byte_blk = sbrk(0);
  if(sbrk((metaSize + size)) == (void *) -1){
    return NULL;
  }
  heap_size = heap_size + metaSize + size;
  head = first_byte_blk;
  tail = head;
  Node_t temp= {.next = NULL, .prev = NULL, .blk_num = size, .log_prev = NULL, .log_next = NULL};
  *first_byte_blk = temp;
  return (void *) (first_byte_blk + 1);
}

void * add_new_segment(Node_t * curr, size_t size){
  Node_t * first_byte_blk = sbrk(0);
  if(sbrk(size + metaSize) == (void *) -1){
    return NULL;
  }
  heap_size = heap_size + metaSize + size;
  Node_t temp = {.next = NULL, .prev = NULL, .blk_num = size, .log_prev = curr, .log_next = NULL};
  curr->log_next = first_byte_blk;
  tail = first_byte_blk; //change tail
  *first_byte_blk = temp;
  return (void *) (first_byte_blk + 1);
}

void * split_and_insert(Node_t * curr, size_t size){
  Node_t * new_meta_info = (Node_t *)( (int8_t *)curr + size + metaSize); //curr address + request bytes + Node_t bytes                                                                 // is actually position of next free block
  Node_t temp = {.next = NULL, .prev = NULL, .blk_num = curr->blk_num - size - metaSize, .log_prev = curr, .log_next = curr->log_next};
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
  curr->blk_num = size;
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



void find_best_position(size_t size, Node_t ** pt_pt_best, size_t * pt_best_size){
  //preventing other threads from modifying the head
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








  
void merge_first_part(Node_t * mergeHead){
  mergeHead->blk_num = mergeHead->blk_num + metaSize + mergeHead->log_next->blk_num;
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
  


void addToFreeList(Node_t * toAdd){
  toAdd->next = free_head;
  if(free_head != NULL){
    free_head->prev = toAdd;
  }
  free_head = toAdd;
}



    

//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size){
  if (size == 0){
    return NULL;
  }
  else{
    void * ans;
    pthread_mutex_lock(&sbrk_lock);
    Node_t * new_blk = sbrk(metaSize + size);
    pthread_mutex_unlock(&sbrk_lock);
    new_blk = 


void ts_free_nolock(void *ptr);




































//performance study auxiliary function

unsigned long get_data_segment_size(){
  return heap_size;
}

unsigned long get_data_segment_free_space_size(){
  Node_t* cur = free_head;
  unsigned long sum = 0;
  while(cur != NULL){
    sum = sum + cur->blk_num /*+ metaSize*/;
    cur = cur->next;
  }
  return sum;
}
