#include <stdio.h>
//use telnet 127.0.0.1 port to connect

#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 255
#define DEFAULT_PORT 23

FILE* file;
int port;
int num_lines;
char **dict;



void readDict();

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
    //printf("dict[%d]: %s", 0, dict[0]);

    return 0;
}

void readDict() { //reads dictionary and stores it in dict**
    char buff[BUFF_SIZE];
    int line_counter = 0;
    while (fgets(buff, BUFF_SIZE, (FILE*) file)) {
        line_counter++;
    }
    printf("line counter: %d", line_counter);
    rewind(file);
    dict = (char**) malloc(sizeof(char *) * line_counter); //dynamically allocate memory for dictionary
    num_lines = 0;
    while(fgets(buff, 255, (FILE*) file)) {
        char* copy = (char *) malloc(sizeof(char) * BUFF_SIZE);
        strcpy(copy, buff);
        dict[num_lines] = copy;
        num_lines++;
    }
}
