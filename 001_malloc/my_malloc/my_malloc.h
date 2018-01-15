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
  Node_t * next;
  Node_t * prev;
  size_t blk_num;
  int isFree;
}; 

//typedef struct _Node_t Node_t;
 
extern size_t heap_size;
extern Node_t * head;
/*
extern size_t heap_size = 0;
extern size_t cur_add_size = 0;
*/
#endif
