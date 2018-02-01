#ifndef _MY_MALLOC__
#define _MY_MALLOC__
#include<stdlib.h>
/**
 * This program implement a thread safe malloc and free functionality
 * in two versions: lock based and lock free
 * This program implements malloc by keeping a sorted singly linked list to track free blocks 
 * It uses sbrk() system call to increment heap size
 * which is by far considered not thread safe. 
 * So either version of the malloc here will have lock locking the sbrk() system call
 **/


//unsigned long get_data_segment_size();
//unsigned long get_data_segment_free_space_size();



typedef struct _Node_t Node_t;
struct _Node_t{
  Node_t * prev_free;
  Node_t * next_free;
  size_t blk_size;
};

void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void * ptr);
void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);
Node_t * searchFit(Node_t * freeHead, size_t size);
void replaceNode(Node_t ** freeHead, Node_t * beReplaced, Node_t * toAdd);
void * split(Node_t ** freeHead, Node_t * toSplit, size_t size);

void merge(Node_t * first, Node_t * second);

void addNode(Node_t ** freeHead, Node_t * toAdd);

#endif
