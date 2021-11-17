#include "producer.h"
extern packets *front;
extern packets *back;
extern sem_t sem_mutex;
extern sem_t sem_cond; 
/**
 *
 * Producer thread will read from the file and write data to 
 * the end of the shared queue
 */
struct packets {
    char *dataLine;
    packets *next;
} typedef struct packets queue;

int isempty(queue *p){
    return (p->next == NULL);
}

void *producer(void *arg){
    printf("producer\n");
    
    //TODO: open the file and read its content line by line
    //Send data to the shared queue
    //When reaching the end of the file, send EOF message
    FILE * fp;
    char * line;
    size_t len = 0;
    ssize_t read;

    fp = fopen(arg, "r");
    if(fp == NULL){
        fprintf(stderr,"Error opening file: %s \n",arg);
        return;
    }

    while((read = getline(&line, &len, fp)) != -1){
        packets *newPacket; 
        read = malloc(sizeof(line));
        newPacket = malloc(sizeof(packets));
        newPacket->dataLine = read;
        if(front == NULL){
            front = newPacket;
        }
    }


    // cleanup and exit
    fclose(fp);

    return NULL; 
}
