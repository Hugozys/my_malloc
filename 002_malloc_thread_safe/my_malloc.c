#include<unistd.h>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "my_malloc.h"
#include<limits.h>
#include<pthread.h>



//define TLS free_head
__thread Node_t * freeHead = NULL;
size_t heap_size = 0;
//define overhead size(meta information)
const size_t metaSize = sizeof(Node_t);

//define global lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


Node_t * searchFit(size_t size){
  Node_t * curr = freeHead;
  //unsigned integer overflowed
  //this will get us the maximum value for size_t
  size_t currBestSize = -1;
  Node_t* currBestPtr = NULL;
  while (curr != NULL){
    if(curr->blk_size == size){
      return curr;
    }
    if(curr->blk_size > size && curr->blk_size < currBestSize){
      currBestPtr = curr;
      currBestSize = curr->blk_size;
    }
    curr = curr->next_free;
  }
  return currBestPtr;
}

void replaceNode(Node_t * beReplaced, Node_t *toAdd){
  if(beReplaced->prev_free == NULL){
    //this is the first node
    freeHead = toAdd;
  }
  else{ //not the first node
    beReplaced->prev_free->next_free = toAdd;
  }
  if(beReplaced->next_free != NULL){
    //this is not the last node
    beReplaced->next_free->prev_free = toAdd;
  }
  toAdd->prev_free = beReplaced->prev_free;
  toAdd->next_free = beReplaced->next_free;
}

void * split(Node_t * toSplit, size_t size){
  Node_t * new_meta_ptr = (Node_t *)((int8_t *)toSplit + metaSize + size);
  Node_t temp = {.prev_free = toSplit->prev_free, .next_free = toSplit->next_free, .blk_size = toSplit->blk_size - size - metaSize};
  *new_meta_ptr = temp;
  //you will need to replace the orignal node inside the free Linked list with the new node that is splitted
  toSplit->blk_size = size; 
  replaceNode(toSplit, new_meta_ptr);
  //you will return the location of the orignal node
  //since that is the space you are going to malloc
  return (void *) (toSplit + 1);
  
}

void deleteNode(Node_t * toDelete){
  if(toDelete->prev_free == NULL){ //head node
    freeHead = toDelete->next_free;
  }
  else{
    toDelete->prev_free->next_free = toDelete->next_free;
  }
  if (toDelete->next_free != NULL){
    toDelete->next_free->prev_free = toDelete->prev_free;
  }
  toDelete->next_free = NULL;
  toDelete->prev_free = NULL;
}


void * ts_malloc_lock(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    void * ans = NULL;
    pthread_mutex_lock(&lock);
    Node_t * bestNode = searchFit(size);
    if(bestNode == NULL){
      //no available area, we need to sbrk()
      Node_t * ptr_new_meta = (Node_t *) sbrk(size + metaSize);
      heap_size = heap_size + size + metaSize;
      ptr_new_meta->prev_free = NULL;
      ptr_new_meta->next_free = NULL;
      ptr_new_meta->blk_size = size;
      ans = (void *)(ptr_new_meta + 1);
      
    }
    else{
      //there is bestNode
      //if we block is big, we need to split the block
      if(bestNode->blk_size >= size + metaSize){
	ans = split(bestNode, size);
      }
      else{
	deleteNode(bestNode);
	ans = (void *) (bestNode + 1);
      }
    }
    pthread_mutex_unlock(&lock);
    return ans;
  }
}   

void merge(Node_t * first, Node_t * second){
  if(first == NULL || second == NULL){return;}
  Node_t * check = (Node_t *)(((int8_t *)(first + 1)) + first->blk_size);
  if(check == second){ //is adjacent
    if(first->next_free->next_free != NULL){
      first->next_free->next_free->prev_free = first;
    }
    first->blk_size = first->blk_size + second->blk_size + metaSize;
    first->next_free = first->next_free->next_free;
    
  }
}

void addNode(Node_t * toAdd){
  Node_t ** curr = &freeHead;
  Node_t * prev = NULL;
  while(*curr != NULL){
    if( *curr > toAdd){ //use address as critical element
      toAdd->next_free = *curr;
      toAdd->prev_free = (*curr)->prev_free;
      (*curr)->prev_free = toAdd;
      *curr = toAdd;
      return;
    }
    prev = *curr;
    curr = &((*curr)->next_free);
  }
  toAdd->prev_free = prev;
  toAdd->next_free = *curr;
  *curr = toAdd;
}


void * ts_malloc_nolock(size_t size){
  if(size == 0){
    return NULL;
  }
  else{
    void * ans = NULL;
    Node_t * bestNode = searchFit(size);
    if(bestNode == NULL){
      //no available area, we need to sbrk()
      pthread_mutex_lock(&lock);
      Node_t * ptr_new_meta = (Node_t *) sbrk(size + metaSize);
      pthread_mutex_unlock(&lock);
      heap_size = heap_size + size + metaSize;
      ptr_new_meta->prev_free = NULL;
      ptr_new_meta->next_free = NULL;
      ptr_new_meta->blk_size = size;
      ans = (void *)(ptr_new_meta + 1);
      
    }
    else{
      //there is bestNode
      //if we block is big, we need to split the block
      if(bestNode->blk_size >= size + metaSize){
	ans = split(bestNode, size);
      }
      else{
	deleteNode(bestNode);
	ans = (void *) (bestNode + 1);
      }
    }
    return ans;
  }
}


void ts_free_nolock(void * ptr){
   if(ptr != NULL){
    Node_t * candidate =  (Node_t *)ptr - 1;
    addNode(candidate);
    Node_t * mergeHead = candidate->prev_free;
    Node_t * mergeTail = candidate->next_free;
    if(mergeHead == NULL){
      merge(candidate, mergeTail);
    }
    else{
      merge(mergeHead, candidate);
      if(mergeHead->next_free == mergeTail){
	merge(mergeHead, mergeTail);
      }
      else{
	merge(candidate, mergeTail);
      }
    }
  }
}




      



void ts_free_lock(void * ptr){
  if(ptr != NULL){
    pthread_mutex_lock(&lock);
    Node_t * candidate =  (Node_t *)ptr - 1;
    addNode(candidate);
    Node_t * mergeHead = candidate->prev_free;
    Node_t * mergeTail = candidate->next_free;
    if(mergeHead == NULL){
      merge(candidate, mergeTail);
    }
    else{
      merge(mergeHead, candidate);
      if(mergeHead->next_free == mergeTail){
	merge(mergeHead, mergeTail);
      }
      else{
	merge(candidate, mergeTail);
      }
    }
    pthread_mutex_unlock(&lock);
  }
}


unsigned long get_data_segment_size(){
  return heap_size;
}

unsigned long get_data_segment_free_space_size(){
  Node_t* cur = freeHead;
  unsigned long sum = 0;
  while(cur != NULL){
    sum = sum + cur->blk_size /*+ sizeof(Node_t)*/;
    cur = cur->next_free;
  }
  return sum;
}
