#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include<unistd.h>
#include <sys/time.h> // for gettimeofday()
#include "queue.h"
#include "codec.h"
#define NUM_THREADS 6
pthread_mutex_t queue_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond2 = PTHREAD_COND_INITIALIZER;

void* print_encrypted(void* arg){
    Queue* q = (Queue*) arg;
    int c = 0;
    pthread_mutex_lock(q->mutex_lock);
    while (!q->first && !q->done){
        pthread_cond_wait(q->first_cond,q->mutex_lock);
    }
    Task *task = q->first;
    pthread_mutex_unlock(q->mutex_lock);
    while (1){//!(q->done && c == q->count)) {
        if ((q->done && c == q->count) || task == NULL){
            printf("get out with c:%d\n",c);
            break;
        }
        printf("now we in task #%d\n",task->index);
        pthread_mutex_lock(task->mutex_task);
        while (!task->encrypted){
            printf("not encrypted\n");
            pthread_cond_wait(task->cond_task,task->mutex_task);
        }
        pthread_mutex_unlock(task->mutex_task);
//        if (!task->encrypted) {
//            printf("not en\n");
//            pthread_mutex_lock(task->mutex_task);
//            pthread_cond_wait(task->cond_task, task->mutex_task);
//            pthread_mutex_unlock(task->mutex_task);
//        }
        pthread_mutex_lock(task->mutex_task);
        if (!q->done || (q->done && c+1 < q->count)){
            while (task->next == NULL){
                printf("next null\n");
                pthread_cond_wait(task->next_task,task->mutex_task);
            }
        }
        Task *next = task->next;
        pthread_mutex_unlock(task->mutex_task);
        c++;
        printf("c:%d, encrypted:%s\n",c,task->str);
        free(task->cond_task);
        free(task->mutex_task);
        free(task->str);
        free(task);
        task = next;
    }
    return NULL;
}
void* process_tasks(void* arg){
    Queue* q = (Queue*) arg;
    Task* task = pop(q);
    while(task != NULL || !q->done){
        if (task != NULL){
            encrypt(task->str,1);
            task->encrypted = 1;
            printf("encrypted task#%d\n",task->index);
            pthread_mutex_lock(task->mutex_task);
            pthread_cond_signal(task->cond_task);
            pthread_mutex_unlock(task->mutex_task);
//            fprintf(stdout, "task %d encrypted:%s\n", task->index,task->str);
////            fflush(stdout);
//            free(task->str);
//            free(task);
//            while (task!=NULL && task->encrypted && task->index == (q->last_print+1)){
//                fprintf(stdout, "encrypted:%s\n", task->str);
//                fflush(stdout);
//                pthread_mutex_lock(q->mutex_lock);
//                q->last_print++;
//                pthread_mutex_unlock(q->mutex_lock);
//                Task *next = task->next;
//                free(task->str);
//                free(task);
//                task = next;
//            }
        }
        else {
            pthread_mutex_lock(&queue_mutex2);
            if (!q->done && q->head == NULL) {
                // wait for new data to be added to the queue
                pthread_cond_wait(&queue_cond2, &queue_mutex2);
            }
            pthread_mutex_unlock(&queue_mutex2);
        }
        task = pop(q);
    }
    return NULL;
}
void* insert_input(void* arg){
    Queue* q = (Queue*) arg;
    char c;
    int counter = 0,c_p = 0;
    int dest_size = 1024;
    char data[dest_size];
    memset(data, 0, dest_size);
    while ((c = getchar()) != EOF){
        data[counter] = c;
        counter++;

        if (counter == 1024){
            char* str = (char*) malloc(sizeof(char) * 1025);
            memset(str, 0, sizeof (char)*1025);
            memcpy(str, data, 1024);
            str[1024] = '\0';
            add(q,str);
            pthread_mutex_lock(&queue_mutex2);
            pthread_cond_signal(&queue_cond2);
            pthread_mutex_unlock(&queue_mutex2);
            counter = 0;
            c_p++;
        }
    }

    if (counter > 0){
        char lastData[counter+1];
        memset(lastData, 0, sizeof (lastData));
        lastData[0] = '\0';
        strncat(lastData, data, counter);
        char* str = (char*) malloc(sizeof(char) * (counter+1));
        memset(str, 0, sizeof (char)*(counter+1));
        memcpy(str, lastData, counter);
        str[counter] = '\0';
        add(q,str);
        c_p++;
    }
    setDone(q);
//    pthread_mutex_lock(&queue_mutex);
//    pthread_cond_broadcast(&queue_cond);
//    pthread_mutex_unlock(&queue_mutex);
    return NULL;
}
int main(){

    Queue* q = initQ();


    pthread_t threads[NUM_THREADS];
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL); // Record start time
    pthread_create(&threads[0], NULL, insert_input, (void*) q);
    for (int i = 1; i < NUM_THREADS-1; ++i) {
        pthread_create(&threads[i], NULL, process_tasks, (void*) q);
    }
    pthread_create(&threads[NUM_THREADS-1], NULL, print_encrypted, (void*) q);
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end_time, NULL); // Record end time
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0; // Calculate elapsed time in seconds

    printf("Elapsed time: %f seconds\n", elapsed_time);
    printf("Q size:%d\n",q->count);
    free(q->mutex_lock);
    free(q->first_cond);
    free(q);
    return 0;
}
