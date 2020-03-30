//
// Created by Priya Nigam on 3/26/20.
//

//change so that it's a queue of file descriptors!

#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>

//to declare queue: struct Queue* queue = makeQueue(#)
Queue* makeQueue(int QUEUE_SIZE) { //returns pointer to queue with initial capacity
    Queue* queue = (Queue*) malloc (sizeof(Queue));
    queue -> cap = QUEUE_SIZE;
    queue -> head = 0;
    queue -> tail = (QUEUE_SIZE -1);
    queue -> size = 0;
    //a queue is going to have an array of jobs
    queue -> array = (int**) malloc(queue-> cap * sizeof(int*));
    return queue;
}

//returns if Queue is empty
int queueEmpty(Queue* q) {
    return (q -> size == 0);
}

//returns if queue is full
int queueFull(Queue* q) {
    if (q-> size == q -> cap) {
        return 1;
    } else {
        return 0;
    }
}

//returns what queue size is
int queueSize(Queue* q) {
    return q->size;
}

//get the first item in queue
int* head (struct Queue* q) {
    if (queueEmpty(q)) {
        return NULL;
    } else {
        return (q-> array[q->head]);
    }
}

//get last item added to queue
//pointer to file descriptor
int* tail(struct Queue* q) {
    if (queueEmpty(q)) {
        return NULL;
    } else {
        return (q-> array[q->tail]);
    }
}

//to declare: Node* cat = new Node(7,2): means data = 7, 2nd priority

//adds item to queue
void add(struct Queue* q, int* fd) {
    if (queueFull(q))
        return;
    q->tail = (q->tail +1) % q->cap;
    q->array[q->tail] = fd;
    q->size=q->size+1;
//    //increase the tail to the next one
//    //store the job as the tail


}

//removes fd from queue and returns it
//this is FIFO so removes the first item in queue
int* del(struct Queue* q) {
    if (queueEmpty(q)) {
        return NULL;
    } else {
        int* fd = (q -> array[q->head]);
        q->head = ((q->head+1)% q ->cap);
        q->size = q->size-1;
        return fd;
    }
}