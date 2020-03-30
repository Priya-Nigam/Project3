

#ifndef PROJECT3_QUEUE_H
#define PROJECT3_QUEUE_H
//
// Created by Priya Nigam on 3/26/20.
//



//create a struct for queue data structure for services
typedef struct Queue {
    int cap;
    int tail;
    int head;
    int size;
    int** array;



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
