#include "header.h"

//global variables
packets *front = NULL;
packets *back = NULL;
double balance[acctsNum];
sem_t sem_mutex;
sem_t sem_mutex2;
sem_t sem_cond;
sem_t sem_logmutex;
int log_option;
FILE * fpl;

/**
 * Write final balance to a single file.
 * The path name should be output/result.txt
 */


void writeBalanceToFiles(void) {
    
    double total = 0.0;
    
    //open final directory output file
    FILE * fp;
    fp = fopen(finalDir,"w");
    if (fp == NULL){
        fprintf(stderr,"ERROR: Cannot open the file %s\n", finalDir);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // write balance for each customer 
    // write total balance change
    
    sem_wait(&sem_mutex2);                      //acquire mutex
    for(int i = 0; i < acctsNum; i++){          //iterate through every account and calculate asset change for bank
        fprintf(fp,"%d\t%lf\n",i,balance[i]);
        total += balance[i];
    }
    sem_post(&sem_mutex2);                      //release mutex

    fprintf(fp,"All: \t%lf\n",total);           //write the total asset change to output file
    fclose(fp);
}

int main(int argc, char *argv[]){
    
    //Argument check
    if(argc < 3){
		fprintf(stderr,"Usage: ./bank #consumers inputFile [option] [#queueSize] \n");
		exit(EXIT_FAILURE);
	}
    if(atoi(argv[1]) < 0 || atoi(argv[1]) > 999){
        fprintf(stderr,"Usage: ./bank #consumers inputFile [option] [#queueSize] \n 0 <= #consumers <= 999");
		exit(EXIT_FAILURE);
    }

    //inputFile check done once file gets opened in producer
    
    //Check to see if log file option flag was input
    if(argc > 3){
        log_option = 0;
        char option[5] = "-p";
        if(strcmp(argv[3], option) == 0){
            log_option = 1;
        }
    }


    bookeepingCode();
    
    //Initialize global variables, especially semaphores
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_mutex2, 0, 1);
    sem_init(&sem_cond, 0, 0);
    sem_init(&sem_logmutex, 0, 1);
    fpl = fopen("output/log.txt", "w");     //fpl refers to log file defined in util.h
    nconsumers = atoi(argv[1]);
    
    
    //create producer and consumer threads
    pthread_t prodtid;
    pthread_t consume_thread[nconsumers];

    //producer
    if(pthread_create(&prodtid, NULL, producer, argv[2]) != 0) {
            printf("Producer thread failed to create\n");
    }else{
        //printf("launching producer\n");
    }

    //consumers
    for(int i = 0; i < atoi(argv[1]); i++) {
        if(pthread_create(&(consume_thread[i]), NULL, consumer, i) != 0) {
            printf("Consumer thread %d failed to create\n", i);
        }
        //printf("launching consumer %d\n", i);
    }
    
    
    //wait for all threads to complete execution and join up
    pthread_join(prodtid, NULL);
    for(int i = 0; i < nconsumers; i++){
        pthread_join(consume_thread[i], NULL);
    }


    //Write the final output
    writeBalanceToFiles();
    
    fclose(fpl);    //close the log file whether or not wrote to it

    return 0; 
}
