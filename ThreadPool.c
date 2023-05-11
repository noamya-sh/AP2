#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/time.h> // for gettimeofday()
#include "queue_heap.h"
#include "codec.h"
#define HEAP_SIZE 20
#define BUFFER_SIZE 1024
pthread_mutex_t queue_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond2 = PTHREAD_COND_INITIALIZER;
typedef struct args{
    Queue *queue;
    min_heap *minHeap;
    int is_encrypt;
    int key;
} args;

int is_all_digits(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

void* print_encrypted(void* arg){
    // print all encrypted chunks by insert order
    args * args1 = (args *) arg;
    Queue *q = args1->queue;
    min_heap *minHeap = args1->minHeap;
    while (!q->done || (q->done && q->last_print < q->count)){
        Task* task = min_heap_extract_min(minHeap,q->last_print);
        if (task != NULL){
            pthread_mutex_lock(q->mutex_lock);
            q->last_print++;
            pthread_mutex_unlock(q->mutex_lock);

            fprintf(stdout, "%s", task->str);
            fflush(stdout);
            free(task->str);
            free(task);
        }
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
    //take chunk from queue and encrypt
    args *args1 = (args*)arg;
    Queue* q = args1->queue;
    min_heap *minHeap = args1->minHeap;
    int is_e = args1->is_encrypt;
    void (*process_func)(char*, int) = (is_e == 1) ? &encrypt : &decrypt;
    while(1){
        Task* task = pop(q);

        if (task != NULL){
            process_func(task->str, args1->key);
            min_heap_insert(minHeap,task);
        }
        else if (!q->done){
            pthread_mutex_lock(&queue_mutex2);
            if (!q->done) {
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
    // take each time chunk of 1024 chars and push to queue
    Queue* q = (Queue*) arg;
    char c;
    int counter = 0;
    int dest_size = BUFFER_SIZE;
    char data[dest_size];
    memset(data, 0, dest_size);
    while ((c = getchar()) != EOF){
        data[counter] = c;
        counter++;

        if (counter == BUFFER_SIZE){
            char* str = (char*) malloc(sizeof(char) * (BUFFER_SIZE+1));
            memset(str, 0, sizeof (char)*(BUFFER_SIZE+1));
            memcpy(str, data, BUFFER_SIZE);
            str[BUFFER_SIZE] = '\0';
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
        pthread_cond_broadcast(&queue_cond2);
        pthread_mutex_unlock(&queue_mutex2);
    }
    setDone(q);
    return NULL;
}
int main(int argc, char *argv[]){
    if (argc != 3){
        printf("usage: key flag \n");
        return 0;
    }
    if (!is_all_digits(argv[1])){
        printf("key should be int\n");
        return 0;
    }
    int is_encrypted,key = atoi(argv[1]);
    char *flag = argv[2];
    if (!strcmp(flag,"-e"))
        is_encrypted = 1;
    else if (!strcmp(flag,"-d"))
        is_encrypted = 0;
    else {
        printf("valid flag is -e or -d!\n");
        return 0;;
    }
    long NUM_THREADS = 12;
    Queue* q = initQ();
    min_heap *minHeap = min_heap_init(HEAP_SIZE);
    args args1 = {q,minHeap,is_encrypted,key};
    pthread_t threads[NUM_THREADS];

//    struct timeval start_time, end_time;
//    gettimeofday(&start_time, NULL); // Record start time
    pthread_create(&threads[0], NULL, insert_input, (void*) q);
    for (int i = 1; i < NUM_THREADS-1; ++i) {
        pthread_create(&threads[i], NULL, process_tasks, (void*) &args1);
    }
    pthread_create(&threads[NUM_THREADS-1], NULL, print_encrypted, (void*) &args1);
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
//    gettimeofday(&end_time, NULL); // Record end time
//    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0; // Calculate elapsed time in seconds

//    printf("Elapsed time: %f seconds\n", elapsed_time);
    free(q->mutex_lock);
    free(q->first_cond);
    free(q);
    min_heap_destroy(minHeap);
    pthread_mutex_destroy(&queue_mutex2);
    pthread_cond_destroy(&queue_cond2);
    return 0;
}
