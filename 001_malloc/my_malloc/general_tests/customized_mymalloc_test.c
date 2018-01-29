#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif
void print_Node_t(Node_t * ptNode_t){
  printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
  Node_t* cur = ptNode_t;
  while(cur != NULL){
    printf("===================================\n");
    printf("address of curr Node_t: %p\n",cur);
    printf("address of next Node_t: %p\n",cur->next);
    printf("address of prev Node_t: %p\n",cur->prev);
    printf("avalible blocks number for this region:%lu\n",cur->blk_num);
    printf("address of next conseutive block: %p\n",cur->log_next);
    printf("address of prev consecutive block: %p\n",cur->log_prev);
    printf("address of its next adjacent region: %p\n",(int8_t *)(cur + 1) + cur->blk_num);
    cur = cur->next;
  }
}
void print_asterik(int k,int a){
  printf("\n");
  for(int i = 0; i < k; ++i){
    printf("*");
  }
  printf("Test case %d",a);
  for(int i =0; i< k; ++i){
    printf("*");
  }
  printf("\n");
}

void blk_test_01_null(){
  print_asterik(10,1);
  char * array = MALLOC(0*sizeof(*array));
  if(array == NULL){
    printf("test passed\n");
  }
  else{
    printf("test failed\n");
  }
}

void blk_test_02_double_free(){
  print_asterik(10,2);
  char * array = MALLOC(5*sizeof(*array));
  FREE(array);
  FREE(array);
}

char * blk_test_03_create_char_space(){
  print_asterik(10,3);
  char * array = MALLOC(5*sizeof(*array));
  if(heap_size == 45){
    printf("test passed\n");
    return array;
  }
  else{
    printf("test failed, heapsize is %lu\n",heap_size);
    return NULL;
  }
}
void blk_test_04_store_and_print(char * array){
  print_asterik(10,4);
  char it = 'A';
  for(int i = 0; i< 5; ++i){
    array[i] = ++it;
    printf("array[%d] = %c\n",i,it);
  }
}

void blk_test_05_free_and_malloc(char *array){
  print_asterik(10,5);
  FREE(array);
  array = MALLOC(3*sizeof(*array));
  printf("Heap Size is: %lu\n",heap_size);
  Node_t * cur = head;
  print_Node_t(cur);
  printf("================================\n");
  print_Node_t(free_head);
}
  
void blk_test_06_merge_adjacent(char * array){
  print_asterik(10,6);
  char * array2 = MALLOC(10*sizeof(*array2));
  char * array3 = MALLOC(20*sizeof(*array3));
  printf("Heap Size is: %lu\n",heap_size); //131 bytes
  Node_t * cur = head;
  printf("**********After malloc finished**********\n");
  print_Node_t(cur);
  FREE(array);
  FREE(array2);
  printf("**********After free finished**********\n");
  cur = head;
  print_Node_t(cur);
  print_Node_t(free_head);
  FREE(array3);
  printf("*********Now***************\n");
  print_Node_t(head);
  print_Node_t(free_head);
}
    
void blk_test_07_ff_algorithm(){
  print_asterik(10,7);
  char * array1 = MALLOC(5*sizeof(*array1));
  char * array2 = MALLOC(10*sizeof(*array2));
  char * array3 = MALLOC(20*sizeof(*array3));
  FREE(array1);
  FREE(array3);
  printf("***********Initial State***********\n");
  printf("heap size: %lu\n",heap_size);
  print_Node_t(head);
  char * array4 = MALLOC(6*sizeof(*array4));
  printf("***********Final State************\n");
  print_Node_t(head);
  FREE(array2);
  FREE(array4);
}
void blk_test_08_merge_adjacent_ver_2(){
  print_asterik(10,8);
  char * array1 = MALLOC(5*sizeof(*array1));
  char * array2 = MALLOC(10*sizeof(*array2));
  char * array3 = MALLOC(20*sizeof(*array3));
  FREE(array1);
  FREE(array3);
  print_Node_t(head);
  printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
  FREE(array2);
  print_Node_t(head);
}
void blk_test_09_increment_heap(){
  print_asterik(10,9);
  printf("***********Initial State***********\n");
  printf("heap size: %lu\n",heap_size);
  print_Node_t(head);
  char * array1 = MALLOC(300*sizeof(*array1));
  printf("***********Expanded State***********\n");
  printf("heap size: %lu\n",heap_size);
  print_Node_t(head);
  FREE(array1);
}
int main(int argc,char * argv[]){
  FREE((void *)0x01);
  //blk_test_01_null();
  //blk_test_02_double_free();
  //char * arr = blk_test_03_create_char_space();
  //blk_test_04_store_and_print(arr);
  //blk_test_05_free_and_malloc(arr);
  //blk_test_06_merge_adjacent(arr);
  //blk_test_07_ff_algorithm();
  //blk_test_08_merge_adjacent_ver_2();
  //blk_test_09_increment_heap();
  return (EXIT_SUCCESS);
}
