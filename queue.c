#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#define Q_ARR_SIZE 50
Queue *initQ(){
    Queue *q = (Queue*) malloc(sizeof (Queue));
    q->head=NULL;
    q->tail=NULL;
    q->first=NULL;
    q->count=0;
    q->size=0;
    q->bool_first=0;
    q->done = 0;
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
    task->index=0;
    task->str = "";
    task->encrypted=0;
    task->next=NULL;
    task->cond_task = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(task->cond_task, NULL) != 0){
        printf("Cond initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    task->next_task = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(task->next_task, NULL) != 0){
        printf("Cond initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    task->mutex_task = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(task->mutex_task, NULL) != 0){
        printf("Mutex initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    return task;
}
void push(Queue *q, Task* t){
    pthread_mutex_lock(q->mutex_lock);
    if (!q->head){
        q->head = t;
    }
    else{
        pthread_mutex_lock(t->mutex_task);
        q->tail->next = t;
        pthread_cond_signal(t->next_task);
        pthread_mutex_unlock(q->mutex_lock);
    }
    if (!q->bool_first){
        q->bool_first = 1;
        q->first = t;
        pthread_cond_broadcast(q->first_cond);
//        pthread_mutex_unlock(q->mutex_lock);
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
Task *getFirst(Queue *q){
    pthread_mutex_lock(q->mutex_lock);
    Task *ans = NULL;
    while (!q->done && !q->first) {
        pthread_cond_wait(q->first_cond,q->mutex_lock);
    }
    if (q->first){
        ans = q->first;
        q->first = q->first->next;
    }
    pthread_mutex_unlock(q->mutex_lock);
    if (!ans) printf("Qsize:%d\n",q->size);
    return ans;
}

void setDone(Queue *q){
    pthread_mutex_lock(q->mutex_lock);
    q->done = 1;
    pthread_cond_signal(q->first_cond);
    pthread_mutex_unlock(q->mutex_lock);
}