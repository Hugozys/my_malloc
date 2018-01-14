#ifndef _MY_MALLOC__
#define _MY_MALLOC__
// first fit malloc and free pair
void * ff_malloc(size_t size);
void ff_free(void * ptr);

//best fit malloc and free pair
void * bf_malloc(size_t size);
void bf_free(void *ptr);

typedef struct _Node_t{
  Node_t * next;
  Node_t * prev;
  int blk_size;
  int isFree;
} Node_t;
 

extern Node_t * head = NULL;
/*
extern size_t heap_size = 0;
extern size_t cur_add_size = 0;
*/
#endif
