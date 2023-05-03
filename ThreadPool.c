#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h> // for gettimeofday()
#include "queue_heap.h"
#include "codec.h"
#define NUM_THREADS 6
pthread_mutex_t queue_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond2 = PTHREAD_COND_INITIALIZER;
typedef struct args{
    Queue *queue;
    min_heap *minHeap;
} args;
void* print_encrypted2(void* arg){
    args * args1 = (args *) arg;
    Queue *q = args1->queue;
    min_heap *minHeap = args1->minHeap;
    int c = 0;
    while (!q->done || (q->done && c < q->count)){
        Task* task = min_heap_extract_min(minHeap,q->last_print);
        pthread_mutex_lock(q->mutex_lock);
        q->last_print++;
        pthread_mutex_unlock(q->mutex_lock);
        fprintf(stdout, "c:%d task:%d encrypted:%s\n", c,task->index,task->str);
        fflush(stdout);
        free(task->str);
        free(task);
        if (q->done && q->last_print == q->count){
            pthread_mutex_lock(&minHeap->mutex);
            minHeap->finished = 1;
            pthread_cond_broadcast(&minHeap->not_empty);
            pthread_mutex_unlock(&minHeap->mutex);
            return NULL;
        }
    }
    return NULL;
}
void* process_tasks(void* arg){
    args *args1 = (args*)arg;
    Queue* q = args1->queue;
    min_heap *minHeap = args1->minHeap;

    while(1){
        Task* task = pop(q);
        if (task != NULL){
            encrypt(task->str,1);
            task->encrypted = 1;
            min_heap_insert(minHeap,task);
        }
        else if (!q->done){
            pthread_mutex_lock(&queue_mutex2);
            if (!q->done && q->head == NULL) {
                pthread_cond_wait(&queue_cond2, &queue_mutex2);
            }
            pthread_mutex_unlock(&queue_mutex2);
        } else{
            break;
        }
    }
    return NULL;
}
void* insert_input(void* arg){
    Queue* q = (Queue*) arg;
    char c;
    int counter = 0;
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
        pthread_mutex_lock(&queue_mutex2);
        pthread_cond_signal(&queue_cond2);
        pthread_mutex_unlock(&queue_mutex2);
    }
    setDone(q);
    return NULL;
}
int main(){

    Queue* q = initQ();
    min_heap *minHeap = min_heap_init(20);
    args args1 = {q,minHeap};
    pthread_t threads[NUM_THREADS];
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL); // Record start time
    pthread_create(&threads[0], NULL, insert_input, (void*) q);
    for (int i = 1; i < NUM_THREADS-1; ++i) {
        pthread_create(&threads[i], NULL, process_tasks, (void*) &args1);
    }
    pthread_create(&threads[NUM_THREADS-1], NULL, print_encrypted2, (void*) &args1);
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
    min_heap_destroy(minHeap);
    pthread_mutex_destroy(&queue_mutex2);
    pthread_cond_destroy(&queue_cond2);
    return 0;
}
