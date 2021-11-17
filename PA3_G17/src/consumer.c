#include "consumer.h"
#include <ctype.h>

extern packets *front;
extern packets *back;
double balance[acctsNum];
extern sem_t sem_mutex;
extern sem_t sem_mutex2;
extern sem_t sem_cond;
/**
 * parse lines from the queue, calculate balance change
 * and update to global array
 */
void parse(char *line){
    int customer_id;
    float transaction;
    float transaction_total = 0;
    // TODO: get customer id
    // TODO: sum up transactions
    // TODO: update the global array
    char *tmp;
    tmp = strtok(line,",");
    customer_id = atoi(tmp);

    while(tmp != NULL){
        tmp = strtok(NULL,",");
        transaction = atof(tmp);
        transaction_total += transaction;
    }

    sem_wait(&sem_mutex2);
    balance[customer_id] += transaction_total;
    sem_post(&sem_mutex2);

}


// consumer function
void *consumer(void *arg){
    printf("consumer %d\n", *(int *)arg);
    //TODO: keep reading from queue and process the data
    // feel free to change
    char linebuf[chunkSize];
    while(1){
        sem_wait(&sem_cond);
        sem_wait(&sem_mutex);
        strcpy(linebuf, front->dataLine);
        if(strstr(linebuf, "EOF") != NULL){
            packets *temp = front;
            front = front->next;
            free(temp);
            sem_post(&sem_mutex);
            return NULL;
        }
        packets *temp = front;
        front = front->next;
        free(temp);
        sem_post(&sem_mutex);
        parse(linebuf);
    }
    
    return NULL; 
}


