//
// Created by Priya Nigam on 4/5/20.
//

#ifndef PROJECT3_LOG_QUEUE_H
#define PROJECT3_LOG_QUEUE_H

#include <pthread.h>

//create a struct for queue data structure for services
typedef struct LogQueue {
    int cap; //max number of slots
    int tail; //array[rear % cap] i last item
    int head; //array[(front+1)%cap] is first item
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t empty_slot;
    pthread_cond_t full_slot;
    char** array; //array of strings


}LogQueue;

LogQueue* makeLogQueue(int INIT_QUEUE_CAP);
int LqueueEmpty(LogQueue* q);
int LqueueFull(LogQueue* q);
int LqueueSize(LogQueue* q);
char* Lhead (struct LogQueue* q);
char* Ltail(struct LogQueue* q);
void L_add(struct LogQueue* q, char* buff);
char* L_del(struct LogQueue* q);






#endif //PROJECT3_LOG_QUEUE// _H
