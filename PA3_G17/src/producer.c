#include "producer.h"


//global variables
extern packets *front;
extern packets *back;
extern sem_t sem_mutex;
extern sem_t sem_cond;
extern sem_t sem_logmutex;
extern int nconsumers;
extern int log_option;
extern FILE * fpl; 



/**
 *
 * Producer thread will read from the file and write data to 
 * the end of the shared queue
 */


void *producer(char *file){
    

    //handle log file if needed
    if(log_option == 1){
        char buf[50];
        sprintf(buf,"producer\n");
        sem_wait(&sem_logmutex);
        writeLineToFile("output/log.txt", buf);
        fflush(fpl);
        sem_post(&sem_logmutex);
    }

        
    
    //open the file and read its content line by line
    //Send data to the shared queue
    //When reaching the end of the file, send EOF message
    
    //define necessary variables
    FILE * fp;
    char line[chunkSize];
    char *b = line;
    size_t len = chunkSize;
    ssize_t read;

    //open file in read mode and handle error
    fp = fopen(file, "r");
    if(fp == NULL){
        fprintf(stderr,"Error opening file: %s \n",file);
        return NULL;
    }


    //read all lines and place at the back of queue
    //once EOF is reached, send necessary amount of EOF packets to the rear of the queue
    int linecounter = 0;
    while(getline(&b, &len, fp) != -1){
        
        //handle log file if necessary
        if(log_option == 1){
            char buf[50];
            sprintf(buf,"producer: line %d\n", linecounter);
            sem_wait(&sem_logmutex);
            writeLineToFile("output/log.txt", buf);
            fflush(fpl);
            sem_post(&sem_logmutex);
        }
            
        //build new packet    
        packets *newPacket = malloc(sizeof(packets));
        newPacket->next = NULL;
        strcpy(newPacket->dataLine, line);
        newPacket->linenum = linecounter;
        
        
        //acquire mutex and place new packet at the rear of the queue
        packets *current = NULL;
        sem_wait(&sem_mutex);
        if(front == NULL){
            front = newPacket;
        }else{
            current = front;
            while(current->next != NULL){
                current = current->next;
            }
            current->next = newPacket;
        }
        sem_post(&sem_mutex);
        sem_post(&sem_cond);
        linecounter++;
    }
    

    //send appropriate number of EOF packets to the rear of the queue
    packets *current = NULL;
    //printf("CONSUMERS: %d", nconsumers);
    for(int i = 0; i < nconsumers; i++){
        //printf("Added EOF");
        packets *eofPacket = malloc(sizeof(packets));
        eofPacket->linenum = -1;
        eofPacket->next = NULL;
        strcpy(eofPacket->dataLine, "EOF");
        sem_wait(&sem_mutex);
        if(front == NULL){
            front = eofPacket;
        }else{
            current = front;
            while(current->next != NULL){
                current = current->next;
            }
            current->next = eofPacket;
        }
        sem_post(&sem_mutex);
        sem_post(&sem_cond);
    }
    
    
    // cleanup and exit
    fclose(fp);
    
    return NULL; 
}
