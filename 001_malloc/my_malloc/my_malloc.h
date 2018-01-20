#ifndef _MY_MALLOC__
#define _MY_MALLOC__
// first fit malloc and free pair
#include<stdlib.h>
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//best fit malloc and free pair
void * bf_malloc(size_t size);
void bf_free(void *ptr);

//performance study auxiliary function
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();



typedef struct _Node_t Node_t;
struct _Node_t{
  Node_t * next; //free
  Node_t * prev; //free
  size_t blk_num;
  Node_t * log_prev; //its consecutive prev
  Node_t * log_next; //its consecutive next
}; 


//typedef struct _Node_t Node_t;
extern Node_t * tail;
extern Node_t * free_head; 
extern size_t heap_size;
extern Node_t * head;
void addToFreeList(Node_t * toAdd);
void deleteNode(Node_t * toRemove);
/*
extern size_t heap_size = 0;
extern size_t cur_add_size = 0;
*/
#endif
