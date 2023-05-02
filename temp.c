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
        encrypt(task->str,1);
        task->encrypted = 1;
        while (task!=NULL && task->encrypted && task->index == (q->last_print+1)){
            printf("encrypted:%s\n",task->str);
            pthread_mutex_lock(q->mutex_lock);
            q->last_print++;
            pthread_mutex_unlock(q->mutex_lock);
            Task *next = task->next;
            free(task->str);
            free(task);
            task = next;
        }
        task = pop(q);
    }
}

int main(){

    Queue* q = initQ();
//    for(int i=0; i<20; i++){
//        char* str = (char*) malloc(sizeof(char) * 10);
//        sprintf(str, "task_%d", i);
//        add(q, str);
//    }
    int key = 1;
    printf("key is %i \n",key);

    char c;
    int counter = 0;
    int dest_size = 1024;
    char data[dest_size];


    while ((c = getchar()) != EOF){
        data[counter] = c;
        counter++;

        if (counter == 1024){
            char* str = (char*) malloc(sizeof(char) * 1024);
            strcpy(str,data);
//            str[1024]='\0';
            add(q,str);
//            encrypt(data,key);
//            printf("encripted data: %s\n",data);
            counter = 0;
        }
    }

    if (counter > 0){
        char lastData[counter];
        lastData[0] = '\0';
        strncat(lastData, data, counter);
        char* str = (char*) malloc(sizeof(char) * (counter));
        strcpy(str,lastData);
        add(q,str);
//        encrypt(lastData,key);
//        printf("encripted data:\n %s\n",lastData);
    }
    pthread_t t1, t2, t3,t4;
    pthread_create(&t1, NULL, process_tasks, (void*) q);
    pthread_create(&t2, NULL, process_tasks, (void*) q);
    pthread_create(&t3, NULL, process_tasks, (void*) q);
    pthread_create(&t4, NULL, process_tasks, (void*) q);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    free(q->mutex_lock);
    free(q);
    return 0;
}
