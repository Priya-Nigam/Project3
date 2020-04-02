#include <stdio.h>
//use telnet 127.0.0.1 port to connect

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Queue.h"
#include <pthread.h>

#define BUFF_SIZE 255
#define DEFAULT_PORT 23
#define WORK_QUEUE_SIZE 5
#define LOG_QUEUE_SIZE 5
#define NUM_THREADS 3

//Global
FILE* file; //dict fd
int port; //port
int num_lines; //number of lines in dict
char **dict; //stores words in dict
Queue* work_queue; //stores socket descriptors
Queue* log_queue; //stores log



void readDict();
_Bool spellChecker(char* word);

int main(int argc, char **argv) {
    if (argc < 2) { //if no command line arguments, uses default dictionary + port else uses given dict + port
        file = fopen("dictionary.txt", "r");
        port = DEFAULT_PORT;
    } else if (argc == 2) {
        file = fopen(argv[1], "r");
        port = DEFAULT_PORT;
    } else if (argc == 3) {
        file = fopen(argv[1], "r");
        port = (int) argv[2];
    } else {
        printf("Error opening file or connecting to port.\n");
    }

    if (file == NULL) {
        printf("Error, cannot read dictionary file.");
        exit(1);
    }
    readDict(); //reads dictionary file and stores it in char** dict
    work_queue = makeQueue(WORK_QUEUE_SIZE); //fixed queue of client socket descriptors
    log_queue = makeQueue(LOG_QUEUE_SIZE); //fixed cue of log

    //after we make the dict data struct + both queues, make threads
    pthread_t threads[NUM_THREADS];
    void *ret;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL,func, arg of func) != 0) {
            printf("Error creating thread. \n");
            return(EXIT_FAILURE);
        }
    }

    char* word = (char*) malloc(sizeof(char) * BUFF_SIZE);
    char buff[BUFF_SIZE];
    //word = "A";
//    while (fscanf(stdin, buff)) {
//        spellChecker(word);
//    }

    //printf("dict[%d]: %s", 0, dict[0]);

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], &ret) != 0) {
            printf("Join error.\n");
            return(EXIT_FAILURE);
        }
    }

    return 0;
}

void *func(void *arg) {
    char *name = (char*) arg;
}

void readDict() { //reads dictionary and stores it in dict**
    char buff[BUFF_SIZE];
    int line_counter = 0;
    while (fgets(buff, BUFF_SIZE, (FILE*) file)) {
        line_counter++;
    }
    rewind(file);
    dict = (char**) malloc(sizeof(char *) * line_counter); //dynamically allocate memory for dictionary
    num_lines = 0;
    while(fgets(buff, 255, (FILE*) file)) {
        char* token = strtok(buff, "\n");
        char* copy = (char *) malloc(sizeof(char) * BUFF_SIZE);
        strcpy(copy, token);
        dict[num_lines] = copy;
        num_lines++;
    }
}

_Bool spellChecker(char* word) {
    int in_dictionary = 0; //0 is false 1 is true
    char* token = strtok(word, "\n");
    for (int i = 0; i < num_lines; i++) {
        if (strcmp(token, dict[i]) == 0) { //are the same
            in_dictionary = 1;
            printf(" %s is in dictionary!", word);
        }
    }
    return in_dictionary;
}