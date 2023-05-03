#ifndef UNTITLED2_QUEUE_H
#define UNTITLED2_QUEUE_H
#include <pthread.h>
typedef struct Task{
    char *str;
    int encrypted;
    int index;
    pthread_mutex_t *mutex_task;
    pthread_cond_t *cond_task;
    pthread_cond_t *next_task;
    struct Task* next;
} Task;
typedef struct Queue{
    Task *head;
    Task *tail;
    Task *first;
    int bool_first;
    int count;
    int size;
    int done;
    pthread_mutex_t *mutex_lock;
    pthread_cond_t *first_cond;
} Queue;
void encrypt(char *s,int key);
Queue* initQ();
Task *initT();
void push(Queue *q, Task* t);
Task* pop(Queue *q);
Task* getFirst(Queue *q);
void add(Queue *q,char * str);
void setDone(Queue *q);
#endif //UNTITLED2_QUEUE_H
