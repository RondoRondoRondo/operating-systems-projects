#include "header.h"

#define NCONSUMERS 3

packets *front = NULL;
packets *back = NULL;
double balance[acctsNum];
sem_t sem_mutex;
sem_t sem_mutex2
sem_t sem_cond;

/**
 * Write final balance to a single file.
 * The path name should be output/result.txt
 */

// create and delete output directory for writeBalanceToFiles

void _createOutputDir(){ 
    mkdir("output", ACCESSPERMS);
}
void _removeOutputDir(){
    pid_t pid = fork();
    if(pid == 0){
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0) {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else{
        wait(NULL);
    }
}


void writeBalanceToFiles(void) {
    
    float total;
    FILE * fp;
    int fp = open(finalDir,"w");
    if (fp == NULL){
        printf("ERROR: Cannot open the file %s\n", fp);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    // TODO: write balance for each customer 
    // TODO: write total balance change
    for(int i = 0; i < acctsNum; i++){
        fprintf(fp,"%d\t%lf\n",i,balance[i]);
        total += balance[i];
    }
    fprintf(fp,"All: \t%lf\n",total);
    fclose(fp);
}

int main(int argc, char *argv[]){
    
    //TODO: Argument check
    if(argc < 3){
		fprintf(stderr,"Usage: ./bank #consumers inputFile [option] [#queueSize] \n");
		exit(EXIT_FAILURE);
	}
    if(argv[1] < 0 || argv[1] > 999){
        fprintf(stderr,"Usage: ./bank #consumers inputFile [option] [#queueSize] \n 0 <= #consumers <= 999");
		exit(EXIT_FAILURE);
    }
    //inputFile check should be done once file gets opened in producer
    char option[5] = "-p";
    if(strcmp(argv[4], option) == 0){
        log_option = 1;
    }

    //remove outputDir, sleep(1), create outputDir
    bookeepingCode();
    
    //TODO: Initialize global variables, like shared queue
        //done above at top of file since they will be global variables
    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_mutex2, 0, 1);
    sem_init(&sem_cond, 0, 0);
    
    //TODO: create producer and consumer threads
    pthread_t prodtid;
    pthread_t consume_thread[NCONSUMERS];

    char *filename = strdup(argv[3]);
    if(pthread_create(&prodtid, NULL, producer, filename) != 0) {
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
