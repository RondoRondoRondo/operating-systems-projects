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
typedef struct packet {
    char *dataLine;
    struct packet *next;
} packets;

// int isempty(queue *p){
//     return (p->next == NULL);
// }

void *producer(char *file, int nconsumers){
    printf("producer\n");
    
    //TODO: open the file and read its content line by line
    //Send data to the shared queue
    //When reaching the end of the file, send EOF message
    FILE * fp;
    char line[chunkSize];
    size_t len = chunkSize;
    ssize_t read;

    fp = fopen(file, "r");
    if(fp == NULL){
        fprintf(stderr,"Error opening file: %s \n",arg);
        return;
    }

    while((read = getline(line, &len, fp)) != -1){
        packets *newPacket = NULL; 
        newPacket = malloc(sizeof(packets));
        strcpy(newPacket->dataLine, line);
        sem_wait(&sem_mutex);
        if(back == NULL){
            front = newPacket;
            back = newPacket;
        }else{
            back->next = newPacket;
            back = back->next;
        }
        sem_post(&sem_mutex);
        sem_post(&sem_cond);
    }

    for()
    // cleanup and exit
    fclose(fp);

    return NULL; 
}
