#include <stdlib.h>
#include <pthread.h>
#include "queue_heap.h"


min_heap * min_heap_init(int capacity) {
    min_heap *heap = (min_heap*)malloc(sizeof(min_heap));
    heap->data = (Task **)malloc(sizeof(int) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    heap->finished = 0;
    pthread_mutex_init(&heap->mutex, NULL);
    pthread_cond_init(&heap->not_empty, NULL);
    pthread_cond_init(&heap->not_full, NULL);
    return heap;
}

void min_heap_insert(struct min_heap *heap, Task* value) {
    pthread_mutex_lock(&heap->mutex);
    // Check if the heap is full.
    while (heap->size == heap->capacity && !heap->finished) {
        pthread_cond_wait(&heap->not_full,&heap->mutex);
    }
    if (heap->finished){
        pthread_mutex_unlock(&heap->mutex);
        return;
    }
    // Insert the new value into the heap.
    heap->data[heap->size] = value;
    int i = heap->size++;
    while (i > 0 && heap->data[i]->index < heap->data[(i - 1) / 2]->index) {
        // Swap the current value with its parent.
        Task* temp = heap->data[i];
        heap->data[i] = heap->data[(i - 1) / 2];
        heap->data[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
    pthread_cond_signal(&heap->not_empty);
    pthread_mutex_unlock(&heap->mutex);
}

Task* min_heap_extract_min(min_heap *heap,int index) {
    pthread_mutex_lock(&heap->mutex);

    // Check if the heap is empty.
    while ((heap->size == 0 && !heap->finished) || heap->data[0]->index > index) {
        // Heap is empty.
        pthread_cond_wait(&heap->not_empty,&heap->mutex);
    }
    if (heap->finished){
        pthread_mutex_unlock(&heap->mutex);
        return NULL;
    }

    // Get the minimum value from the heap.
    Task* min_value = heap->data[0];

    // Remove the minimum value from the heap.
    heap->data[0] = heap->data[--heap->size];
    int i = 0;
    while (i < heap->size / 2) {
        // Swap the current value with its smaller child.
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smaller_child = left;
        if (right < heap->size && heap->data[right]->index < heap->data[left]->index) {
            smaller_child = right;
        }
        if (heap->data[smaller_child]->index < heap->data[i]->index) {
            // Swap the current value with its smaller child.
            Task* temp = heap->data[i];
            heap->data[i] = heap->data[smaller_child];
            heap->data[smaller_child] = temp;
            i = smaller_child;
        } else {
            break;
        }
    }
    pthread_cond_signal(&heap->not_full);
    pthread_mutex_unlock(&heap->mutex);

    return min_value;
}

void min_heap_destroy(struct min_heap *heap) {
    pthread_mutex_lock(&heap->mutex);
    free(heap->data);
    pthread_mutex_unlock(&heap->mutex);
    pthread_mutex_destroy(&heap->mutex);
    pthread_cond_destroy(&heap->not_empty);
    pthread_cond_destroy(&heap->not_full);
    free(heap);
}
//void* t1_f(void* arg){
//    min_heap *heap = (min_heap *)arg;
//    min_heap_insert(heap, 10);
//    min_heap_insert(heap, 2);
//    min_heap_insert(heap, 5);
//    min_heap_insert(heap, 7);
//    return NULL;
//}
//void* t2_f(void* arg){
//    min_heap *heap = (min_heap *)arg;
//    for (int i = 0; i < 8; ++i) {
//        int min_value = min_heap_extract_min(heap);
//        printf("The minimum value is %d\n", min_value);
//    }
//    return NULL;
//}
//void* t3_f(void* arg){
//    min_heap *heap = (min_heap *)arg;
//    sleep(1);
//    heap->finished = 1;
//    pthread_cond_broadcast(&heap->not_empty);
//    pthread_cond_broadcast(&heap->not_full);
//    //send signals to other threads to finish
//    return NULL;
//}
//int main() {
//    // Create a min-heap with a capacity of 10 elements.
//    min_heap *heap = min_heap_init(1);
//    pthread_t t1,t2,t3;
//    pthread_create(&t1,NULL,t1_f,(void*)heap);
//    pthread_create(&t2,NULL,t2_f,(void*)heap);
//    pthread_create(&t3,NULL,t3_f,(void*)heap);
//    pthread_join(t1,NULL);
//    pthread_join(t2,NULL);
//    pthread_join(t3,NULL);
//    // Destroy the heap.
//    min_heap_destroy(heap);
//
//    return 0;
//}