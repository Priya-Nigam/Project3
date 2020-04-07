#include "Log_Queue.h"
#include <stdio.h>
//use telnet 127.0.0.1 port to connect

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Queue.h"
#include "Log_Queue.h"
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUFF_SIZE 255
#define WORK_QUEUE_SIZE 5
#define LOG_QUEUE_SIZE 5
#define NUM_THREADS 3
#define BACKLOG 10
#define DEFAULT_PORT_STR "22222"
#define EXIT_USAGE_ERROR 1
#define EXIT_GETADDRINFO_ERROR 2
#define EXIT_BIND_FAILURE 3
#define EXIT_LISTEN_FAILURE 4
#define MAX_LINE 64

//Global
FILE* file; //dict fd
char* port; //port
int num_lines; //number of lines in dict
char **dict; //stores words in dict
Queue* work_queue; //stores socket descriptors
LogQueue* log_queue; //stores log
int log_fd;//log file descriptor


int getlistenfd(char*); //from fiore's notes
ssize_t readLine(int fd, void *buffer, size_t n); //from fiore's notes


void readDict();
_Bool spellChecker(char* word);
void* worker_thread_func(void * arg);
void* server_thread_func(void* arg);
void service_client(int fd);

int main(int argc, char **argv) {
    if (argc < 2) { //if no command line arguments, uses default dictionary + port else uses given dict + port
        file = fopen("dictionary.txt", "r");
        port = DEFAULT_PORT_STR;
    } else if (argc == 2) {
        file = fopen(argv[1], "r");
        port = DEFAULT_PORT_STR;
    } else if (argc == 3) {
        file = fopen(argv[1], "r");
        port = argv[2];
    } else {
        fprintf(stderr,"Error opening file or connecting to port.\n");
    }

    if (file == NULL) {
        fprintf(stderr,"Error, cannot read dictionary file.");
        exit(1);
    }
    readDict(); //reads dictionary file and stores it in char** dict
    work_queue = makeQueue(WORK_QUEUE_SIZE); //fixed queue of client socket descriptors
    log_queue = makeLogQueue(LOG_QUEUE_SIZE); //fixed cue of log
    if ((log_fd = open("log.txt", O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO)) == -1) {
        printf("could not open log file.");
    }

    //after we make the dict data struct + both queues, make threads
    //MAKE THREAD POOL
    pthread_t worker_threads[NUM_THREADS];
    pthread_t server_thread;
    void *ret;
    int i=0;
    char* input;
    input = (char*) malloc(sizeof(char) * BUFF_SIZE);

    int listenfd,	       /* listen socket descriptor */
            connectedfd;       /* connected socket descriptor */
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size;
    char line[MAX_LINE];
    char client_name[MAX_LINE];
    char client_port[MAX_LINE];
    ssize_t  bytes_read;
    //char *port;


    if (pthread_create(&server_thread, NULL,&server_thread_func, NULL) != 0) {
        printf("Error creating server thread. \n");
        return(EXIT_FAILURE);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&worker_threads[i], NULL,&worker_thread_func, NULL) != 0) {
            fprintf(stderr, "Error creating worker thread. \n");
            return(EXIT_FAILURE);
        }
    }


    listenfd=getlistenfd(port);
    //to accept and distribute connection requests

    while(true) {
        client_addr_size=sizeof(client_addr);
        if ((connectedfd=accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_size))==-1) {
            fprintf(stderr, "accept error\n");    //connected_socket = accept(listening_socket);
            continue;
        }
        if (getnameinfo((struct sockaddr*)&client_addr, client_addr_size,
                        client_name, MAX_LINE, client_port, MAX_LINE, 0)!=0) {
            fprintf(stderr, "error getting name information about client\n");
        } else {
            printf("accepted connection from %s:%s\n", client_name, client_port);
        } //gets name information
        add(work_queue, connectedfd);//add connected_socket to the work queue
//          //signal any sleeping workers that there's a new socket in the queue
    }



//    char* word = (char*) malloc(sizeof(char) * BUFF_SIZE);
//    char buff[BUFF_SIZE];
    //word = "A";
//    while (fscanf(stdin, buff)) {
//        spellChecker(word);
//    }

    //printf("dict[%d]: %s", 0, dict[0]);
//

    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(worker_threads[i], &ret) != 0) {
            printf("Join error.\n");
            return(EXIT_FAILURE);
        }
    }
    pthread_join(server_thread, &ret);

    exit(0);
    //return 0;
}

void* server_thread_func(void* arg) {
    char* buff;
    char* tokens;
    while (true) {
        while (LqueueEmpty(log_queue) != true) { //while the queue is not empty
            buff = L_del(log_queue); //remove item from log queue
            //notify that there's an empty spot in the queue
            tokens = strtok(buff, "\n\t");
            tokens = strcat(buff, "\n");
            write(log_fd, tokens, strlen(tokens));
            fsync(log_fd);
            //log file can write it just doesn't write until end
            //exit(0);
        }
    }
    return arg;
}

void* worker_thread_func(void * arg) {
    //printf("hello!\n");
    while (true) {
        while (queueEmpty(work_queue) != true) {
            int socket = del(work_queue);//remove a socket from the queue
            // notify that there's an empty spot in teh queue
            service_client(socket); //service client
            close(socket);//close socket
            printf("connection closed\n");
        }
    }
  return arg;
}

void service_client(int fd) {
    ssize_t bytes_read;
    char word[BUFF_SIZE];
    char* tokens;
    while ((bytes_read=readLine(fd, word, BUFF_SIZE))>0 && (strcmp(word, "QUIT\n") != 0)) { //read word from the socket
//            printf("just read %s\n", word);
//            write(fd, word, strlen(word));
//            L_add(log_queue,word);
            tokens = strtok(word, "\n\t");
            if (spellChecker(tokens)) {
                strcat(tokens, " OKAY\n");
                printf("%s",tokens);
                write(fd, tokens, strlen(tokens)); //echo word back on socket concatenated with OK
            } else {
                strcat(tokens, " MISSPELLED\n");
                printf("%s",tokens);
                write(fd, tokens, strlen(tokens)); //echo word back on socket concatenated with "MISSPELLED"
            }
            L_add(log_queue, tokens); //write word and socket response value (ok or mispelled) to log queue
    }
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
           // printf(" %s is in dictionary!", word);
        }
    }
    return in_dictionary;
}





/* given a port number or service as string, returns a
   descriptor that we can pass to accept() */ //from FIORE
int getlistenfd(char *port) {
    int listenfd, status;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    hints.ai_family = AF_INET;	   /* IPv4 */

    if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error %s\n", gai_strerror(status));
        exit(EXIT_GETADDRINFO_ERROR);
    }

    /* try to bind to the first available address/port in the list.
       if we fail, try the next one. */
    for(p = res;p != NULL; p = p->ai_next) {
        if ((listenfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol))<0) {
            continue;
        }

        if (bind(listenfd, p->ai_addr, p->ai_addrlen)==0) {
            break;
        }
    }
    freeaddrinfo(res);
    if (p==NULL) {
        fprintf(stderr, "bind error. quitting.\n");
        exit(EXIT_BIND_FAILURE);
    }

    if (listen(listenfd, BACKLOG)<0) {
        fprintf(stderr, "listen error. quitting.\n");
        close(listenfd);
        exit(EXIT_LISTEN_FAILURE);
    }
    return listenfd;
}


/* FROM KERRISK (FIORE)

   Read characters from 'fd' until a newline is encountered. If a newline
   character is not encountered in the first (n - 1) bytes, then the excess
   characters are discarded. The returned string placed in 'buf' is
   null-terminated and includes the newline character if it was read in the
   first (n - 1) bytes. The function return value is the number of bytes
   placed in buffer (which includes the newline character if encountered,
   but excludes the terminating null byte). */
ssize_t readLine(int fd, void *buffer, size_t n) {
    ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}
