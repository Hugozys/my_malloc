#ifndef _MY_MALLOC__
#define _MY_MALLOC__
#include<stdlib.h>

//best fit malloc and free pair
void * ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

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
  //phtread_mutex_t node_lock;
};



//typedef struct _Node_t Node_t;
extern Node_t * tail;
extern __thread Node_t * free_head; 
extern size_t heap_size;
extern Node_t * head;
void addToFreeList(Node_t * toAdd);
void deleteNode(Node_t * toRemove);

void * add_new_segment_head(size_t size);
void * add_new_segment(Node_t * curr, size_t size);
void * split_and_insert(Node_t * curr, size_t size);
void deleteNode(Node_t * toRemove);
void find_best_position(size_t size, Node_t ** pt_pt_best, size_t * pt_best_size);
void merge_first_part(Node_t * mergeHead);
void merge_second_part(Node_t * mergeHead);

void addToFreeList(Node_t * toAdd);
#endif
