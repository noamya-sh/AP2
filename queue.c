#include <stdio.h>
#include <stdlib.h>
#include "queue_heap.h"
#define Q_ARR_SIZE 50
Queue *initQ(){
    Queue *q = (Queue*) malloc(sizeof (Queue));
    q->head = q->tail = NULL;
    q->count = q->size = q->last_print = q->done = 0;
    q->first_cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(q->first_cond, NULL) != 0){
        printf("Cond initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    q->mutex_lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(q->mutex_lock, NULL) != 0){
        printf("Mutex initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    return q;
}
Task *initT(){
    Task *task = (Task *) malloc(sizeof (Task));
    task->index = 0;
    task->str = "";
    task->next=NULL;
    return task;
}
void push(Queue *q, Task* t){
    pthread_mutex_lock(q->mutex_lock);
    if (!q->head){
        q->head = t;
    }
    else{
        q->tail->next = t;
    }
    q->tail = t;
    t->index = q->count;
    q->count++;
    q->size++;
    pthread_mutex_unlock(q->mutex_lock);
}
Task *pop(Queue *q){
    pthread_mutex_lock(q->mutex_lock);
    Task *ans = NULL;
    if (q->head){
        ans = q->head;
        q->head = q->head->next;
        q->size--;
    }
    pthread_mutex_unlock(q->mutex_lock);
    return ans;

}
void add(Queue *q,char * str){
    Task * task = initT();
    task->str = str;
    push(q,task);
}

void setDone(Queue *q){
    pthread_mutex_lock(q->mutex_lock);
    q->done = 1;
    pthread_cond_signal(q->first_cond);
    pthread_mutex_unlock(q->mutex_lock);
}