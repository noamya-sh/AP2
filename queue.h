#ifndef UNTITLED2_QUEUE_H
#define UNTITLED2_QUEUE_H
#include <pthread.h>
typedef struct Task{
    char *str;
    int encrypted;
    int index;
    struct Task* next;
} Task;
typedef struct Queue{
    Task *head;
    Task *tail;
    int count;
    int last_print;
    pthread_mutex_t *mutex_lock;
} Queue;
void encrypt(char *s,int key);
Queue* initQ();
Task *initT();
void push(Queue *q, Task* t);
Task* pop(Queue *q);
void add(Queue *q,char * str);
#endif //UNTITLED2_QUEUE_H
