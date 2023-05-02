#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue *initQ(){
    Queue *q = (Queue*) malloc(sizeof (Queue));
    q->head=NULL;
    q->tail=NULL;
    q->count=0;
    q->last_print=-1;
    q->mutex_lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(q->mutex_lock, NULL) != 0){
        printf("Mutex initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    return q;
}
Task *initT(){
    Task *task = (Task *) malloc(sizeof (Task));
    task->index=0;
    task->str = "";
    task->encrypted=0;
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
    pthread_mutex_unlock(q->mutex_lock);
}
Task *pop(Queue *q){
    pthread_mutex_lock(q->mutex_lock);
    Task *ans = NULL;
    if (q->head){
        ans = q->head;
        q->head = q->head->next;
        q->count--;
    }
    pthread_mutex_unlock(q->mutex_lock);
    return ans;

}
void add(Queue *q,char * str){
    Task * task = initT();
    task->str = str;
    push(q,task);
}
