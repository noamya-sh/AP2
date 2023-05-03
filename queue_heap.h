#ifndef UNTITLED2_QUEUE_HEAP_H
#define UNTITLED2_QUEUE_HEAP_H
#include <pthread.h>
typedef struct Task{
    char *str;
    int index;
    struct Task* next;
} Task;
typedef struct Queue{
    Task *head;
    Task *tail;
    int last_print;
    int count;
    int size;
    int done;
    pthread_mutex_t *mutex_lock;
    pthread_cond_t *first_cond;
} Queue;
typedef struct min_heap {
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    Task **data;
    int size;
    int capacity;
    int finished;
} min_heap;
void encrypt(char *s,int key);
Queue* initQ();
Task *initT();
void push(Queue *q, Task* t);
Task* pop(Queue *q);
void add(Queue *q,char * str);
void setDone(Queue *q);
min_heap * min_heap_init(int capacity);
void min_heap_insert(min_heap *heap, Task* value);
Task* min_heap_extract_min(min_heap *heap, int index);
void min_heap_destroy(min_heap *heap);
#endif //UNTITLED2_QUEUE_HEAP_H
