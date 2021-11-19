#include "consumer.h"
#include <ctype.h>

//global variables
extern packets *front;
extern packets *back;
double balance[acctsNum];
extern sem_t sem_mutex;
extern sem_t sem_mutex2;
extern sem_t sem_cond;
extern int log_option;
extern FILE * fpl;



/**
 * parse lines from the queue, calculate balance change
 * and update to global array
 */
void parse(char *line, int linenum, int consnum){
    
    //initialize variables needed for parsing
    int customer_id;
    double transaction = 0.0;
    double transaction_total = 0;
    char str[chunkSize];
    strcpy(str,line);
    char *rest = str;
    // get customer id
    
    
    char *token = strtok_r(line,",", &rest);
    customer_id = atoi(token);
    
    // sum up transactions
    token = strtok_r(NULL, ",", &rest);
    while(token != NULL){
        transaction = atof(token);
        transaction_total += transaction;
        token = strtok_r(NULL, ",", &rest);
    }

    //write log to log file if needed
    if(log_option == 1){
        char buf[50];
        sprintf(buf,"consumer %d: line %d\n", consnum, linenum);
        sem_wait(&sem_logmutex);
        writeLineToFile("output/log.txt", buf);
        fflush(fpl);
        sem_post(&sem_logmutex);
    }
        
    // update the global array
    sem_wait(&sem_mutex2);
    balance[customer_id] += transaction_total;
    sem_post(&sem_mutex2);

}


// consumer function
void *consumer(int arg){
    
    //write to log file if needed
    if(log_option == 1){
        char buf[50];
        sprintf(buf,"consumer %d\n",arg);
        sem_wait(&sem_logmutex);
        writeLineToFile("output/log.txt", buf);
        fflush(fpl);
        sem_post(&sem_logmutex);
    }
        
    
    //keep reading from queue and process the data
    char linebuf[chunkSize];
    while(1){
        int linenum;

        //acquire mutex and wait on condition variable
        sem_wait(&sem_cond);
        sem_wait(&sem_mutex);

        //grab line from the front of the queue
        strcpy(linebuf, front->dataLine);
        
        //grab the line number from the front of the queue
        linenum = front->linenum;
        
        //check if packet is EOF packet
        if(strstr(linebuf, "EOF") != NULL){
            
            //unlink the packet from the queue and free it
            packets *temp = front;
            front = front->next;
            free(temp);
            
            //release mutex
            sem_post(&sem_mutex);
            
            //handle log file if needed
            if(log_option == 1){
                char buf[50];
                sprintf(buf,"consumer %d: line -1\n", arg);
                sem_wait(&sem_logmutex);
                writeLineToFile("output/log.txt", buf);
                fflush(fpl);
                sem_post(&sem_logmutex);
            }
                
            return NULL;
        }//End of EOF check


        //unlink the packet at front of queue and free it
        packets *temp = front;
        front = front->next;
        free(temp);
        
        //release mutex
        sem_post(&sem_mutex);
        
        //parse the valid line
        parse(linebuf, linenum, arg);
    }
    
    return NULL; 
}