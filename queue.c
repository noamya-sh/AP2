#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue *initQ(){
    Queue *q = (Queue*) malloc(sizeof (Queue));
    q->head=NULL;
    q->tail=NULL;
    q->count=0;
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
//    pthread_mutex_lock(q->mutex_lock);
    if (!q->head){
        q->head = t;
        printf("%d\n",q->count);
    }
    else{
        q->tail->next = t;
    }
    q->tail = t;
    q->count++;
//    pthread_mutex_unlock(q->mutex_lock);
}
Task *front(Queue *q){
//    pthread_mutex_lock(q->mutex_lock);
    if (q->head){
        Task *task = q->head;
//        pthread_mutex_unlock(q->mutex_lock);
        return task;
    }
//    pthread_mutex_unlock(q->mutex_lock);
    return NULL;
}
Task *pop(Queue *q){
    pthread_mutex_lock(q->mutex_lock);
    Task *ans = NULL;
    if (q->head){
        ans = q->head;
        q->head = q->head->next;
        q->count--;
        //todo
    }
    pthread_mutex_unlock(q->mutex_lock);
    return ans;

}
void add(Queue *q,char * str){
    Task * task = initT();
    task->str = str;
    push(q,task);
}
