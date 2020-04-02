

#ifndef PROJECT3_QUEUE_H
#define PROJECT3_QUEUE_H
//
// Created by Priya Nigam on 3/26/20.
//

#include <pthread.h>

//create a struct for queue data structure for services
typedef struct Queue {
    int cap; //max number of slots
    int tail; //array[rear % cap] i last item
    int head; //array[(front+1)%cap] is first item
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t empty_slot;
    pthread_cond_t full_slot;
    int** array; //pointer to buffer array


}Queue;

Queue* makeQueue(int INIT_QUEUE_CAP);
int queueEmpty(Queue* q);
int queueFull(Queue* q);
int queueSize(Queue* q);
int* head (struct Queue* q);
int* tail(struct Queue* q);
void add(struct Queue* q, int* fd);
int* del(struct Queue* q);






#endif //PROJECT3_QUEUE_H
