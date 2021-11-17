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
// typedef struct packet {
//     char *dataLine;
//     struct packet *next;
// } packets;

// int isempty(queue *p){
//     return (p->next == NULL);
// }

void *producer(char *file, int nconsumers){
    // if(log_option == 1){
    //     FILE * fpl;
    //     fpl = fopen("output/log.txt", "w");
    //     if(fpl == NULL){
    //         fprintf(stderr,"Error opening log file\n");
    //         return;
    //     }
    //     fprintf(fpl,"producer\n");
    //     fflush(fpl);
    // }
    
    //TODO: open the file and read its content line by line
    //Send data to the shared queue
    //When reaching the end of the file, send EOF message
    FILE * fp;
    char line[chunkSize];
    size_t len = chunkSize;
    ssize_t read;

    fp = fopen(file, "r");
    if(fp == NULL){
        fprintf(stderr,"Error opening file: %s \n",file);
        return;
    }

    int linecounter = 0;
    while((read = getline(line, &len, fp)) != -1){
        // if(log_option == 1){
        //     fprintf(fpl,"producer: line %d\n", linecounter);
        //     fflush(fpl);
        // }
        packets *newPacket = NULL; 
        newPacket = malloc(sizeof(packets));
        strcpy(newPacket->dataLine, line);
        newPacket->linenum = linecounter;
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
        linecounter++;
    }
    
    // if(log_option == 1){
    //         fclose(fpl);
    // }

    for(int i = 0; i < nconsumers; i++){
        packets *eofPacket = malloc(sizeof(packets));
        eofPacket->dataLine = "EOF";
        sem_wait(&sem_mutex);
        if(back == NULL){
            front = eofPacket;
            back = eofPacket;
        }else{
            back->next = eofPacket;
            back = back->next;
        }
        sem_post(&sem_mutex);
        sem_post(&sem_cond);
    }
    // cleanup and exit
    fclose(fp);

    return NULL; 
}
