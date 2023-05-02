#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"
#include "codec.h"

void* process_tasks(void* arg){
    Queue* q = (Queue*) arg;
    Task* task = pop(q);
    while(task != NULL){
        printf("q size:%d\n",q->count);
        char c[10];
        strcpy(c,task->str);
        encrypt(task->str,1);
        printf("%s, encrypted:%s\n",c, task->str);
        free(task->str);
        free(task);
        task = pop(q);

    }
}

int main(){
    Queue* q = initQ();
    for(int i=0; i<20; i++){
        char* str = (char*) malloc(sizeof(char) * 10);
        sprintf(str, "task_%d", i);
        add(q, str);
    }
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, process_tasks, (void*) q);
    pthread_create(&t2, NULL, process_tasks, (void*) q);
    pthread_create(&t3, NULL, process_tasks, (void*) q);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    free(q->mutex_lock);
    free(q);
    return 0;
}
