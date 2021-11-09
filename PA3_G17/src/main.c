#include "header.h"

#define NCONSUMERS 3
/**
 * Write final balance to a single file.
 * The path name should be output/result.txt
 */
void writeBalanceToFiles(void) {
    // TODO: write balance for each customer 
    
    // TODO: write total balance change
}

int main(int argc, char *argv[]){
    
    //TODO: Argument check

    bookeepingCode();
    
    //TODO: Initialize global variables, like shared queue
    
    //TODO: create producer and consumer threads
    pthread_t prodtid;
    pthread_t consume_thread[NCONSUMERS];


    if(pthread_create(&prodtid, NULL, producer, 0) != 0) {
            printf("Producer thread failed to create\n");
    }else{
        printf("launching producer\n");
    }

    for(int i = 0; i < NCONSUMERS; i++) {
        // Create threads
        int *carg = malloc(sizeof(*carg));
        *carg = i;
        if(pthread_create(&(consume_thread[i]), NULL, consumer, carg) != 0) {
            printf("Consumer thread %d failed to create\n", i);
        }
        printf("launching consumer %d\n", i);
        free(carg);
    }
    //TODO: wait for all threads to complete execution
    pthread_join(prodtid, NULL);
    for(int i = 0; i < NCONSUMERS; i++){
        pthread_join(consume_thread[i], NULL);
    }
    //Write the final output
    writeBalanceToFiles();
    
    return 0; 
}