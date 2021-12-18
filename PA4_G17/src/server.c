#include "../include/server.h"

#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9796
#define SA struct sockaddr

int num_accounts;
void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

void doLogging(){
    FILE * fp;
    char tempusername[64];
    char tempname[64];
    int tempacc;
    float tempbal;
    time_t tempbday;
    while(1){
        sleep(5);
        fp = fopen("output/balances.csv", "w");
        if(fp == NULL){
            fprintf(stderr,"Error opening balances.csv\n");
            return;
        }
        for(int i = 0; i < num_accounts; i++){
            sem_wait(&semaphores[i]);
            tempacc = i;
            tempbal = accounts[i].balance;
            strcpy(tempusername,accounts[i].username);
            strcpy(tempname,accounts[i].name);
            tempbday = accounts[i].birthday;
            sem_post(&semaphores[i]);
            //printf("Writing a new log for account %d\n",i);
            fprintf(fp, "%d,%.2f,%s,%s,%ld\n", tempacc,tempbal,tempusername,tempname,tempbday);
        }
        fclose(fp);
    }

    
    return;
}//doLogging


// HELPER FUNCTIONS
//  
//   
//    
//     

int readRegister(int sockfd){
    //printf("Executing readRegister \n");
    //acquire mutex for global num_accounts counter, save the value and increment
    sem_wait(&sem_num);
    int tempnum = num_accounts;
    num_accounts++;
    sem_post(&sem_num);


    //read bytes
    char username[64];
    memset(username, 0, sizeof(username));
    if (read(sockfd, username, sizeof(username)) < 0) {
            perror("cannot read");
            exit(4);
    }
    char name[64];
    memset(name, 0, sizeof(name));
    if (read(sockfd, name, sizeof(name)) < 0) {
            perror("cannot read");
            exit(4);
    }
    time_t birthday;
    if (read(sockfd, &birthday, 8) < 0) {
            perror("cannot read");
            exit(4);
    }
    //printf("Bday: %ld\n", birthday);
    //initialize the mutex for the account
    sem_init(&semaphores[tempnum], 0, 1);

    //acquire mutex for this new account and fill in the fields
    sem_wait(&semaphores[tempnum]);
    strcpy(accounts[tempnum].username,username);
    strcpy(accounts[tempnum].name, name);
    accounts[tempnum].birthday = birthday;
    sem_post(&semaphores[tempnum]);

    return tempnum; //return the number of the account just created for use in sendBalance

}//readRegister



int readGetAccountInfo(int sockfd){
    int temp = 0;
    if (read(sockfd, &temp, sizeof(temp)) < 0) {
            perror("cannot read");
            exit(4);
    }

    return temp;
}//readGetAccountInfo


int readTransact(int sockfd){
    //printf("Executing readTransact\n");
    int account_number = 0;
    if (read(sockfd, &account_number, sizeof(account_number)) < 0) {
            perror("cannot read");
            printf("failed \n");
            exit(4);
    }
    //printf("Check 1 \n");
    float amount = 0.0;
    if (read(sockfd, &amount, sizeof(amount)) < 0) {
            printf("Check 2 \n");
            perror("cannot read");
            exit(4);
    }
    //printf("amount read 380367 = %.2f \n", amount);
    sem_wait(&semaphores[account_number]);
    //printf("Check 2 \n");
    accounts[account_number].balance += amount;
    sem_post(&semaphores[account_number]);
    //printf("readTransact executed \n");
    return account_number;


}//readTransact


int readGetBalance(int sockfd){
    //printf("Executing readGetBalance\n");
    int account_number = 0;
    if (read(sockfd, &account_number, sizeof(account_number)) < 0) {
            perror("cannot read");
            exit(4);
    }

    return account_number;
}//readGetBalance

void sendAccountInfo(int sockfd, int account){
    msg_enum mt = ACCOUNT_INFO;
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    char temp[64];
    strcpy(temp,accounts[account].username);
    if (write(sockfd, &temp, sizeof(temp)) < 0) { //message_type mt
        perror("Cannot write");
        exit(3);
    }
    strcpy(temp, accounts[account].name);
    if (write(sockfd, &temp, sizeof(temp)) < 0) { //message_type mt
        perror("Cannot write");
        exit(3);
    }
    if (write(sockfd, &accounts[account].birthday, sizeof(accounts[account].birthday)) < 0) { //message_type mt
        perror("Cannot write");
        exit(3);
    }
    return;
}

void sendBalance(int sockfd, int account_num){
    //printf("Executing sendBalance\n");
    msg_enum mt = BALANCE;
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    if (write(sockfd, &account_num, sizeof(account_num)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    sem_wait(&semaphores[account_num]);
    float bal = accounts[account_num].balance;
    sem_post(&semaphores[account_num]);
    if (write(sockfd, &bal, sizeof(bal)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    return;
}

float readRequestCash(int sockfd){
    //rintf("Executing readRequestCash\n");
    float amount = 0.0;
    if (read(sockfd, &amount, sizeof(amount)) < 0) {
            perror("cannot read");
            exit(4);
    }

    return amount;
}//readRequestCash

void sendCash(int sockfd, float amount){
    //printf("Executing sendCash\n");
    msg_enum mt = CASH;
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    if (write(sockfd, &amount, sizeof(amount)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    return;
}


void readError(int sockfd){
    int message_type = 0;
    if (read(sockfd, &message_type, sizeof(message_type)) < 0) {
            perror("cannot read");
            exit(4);
    }
    //printf("Error: message %d was received by server", message_type);
    return;
}//readError


// Function designed for chat between client and server.
void func(void *arg) {
    // cast arg to client socket (sockfd)
    int sockfd = *(int *) arg;
    //printf("Func reached\n");
    // parse each query received and reply with the appropriate response.
    msg_enum mt = REGISTER;
    int account_number;
    float temp_amount;
    while(mt != TERMINATE){
        if (read(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("cannot read in func");
            exit(4);
        }
        //printf("Message received: %d\n", mt);
        switch (mt){
            case REGISTER:
                account_number = readRegister(sockfd);
                sendBalance(sockfd, account_number);
                break;
            case GET_ACCOUNT_INFO:
                account_number = readGetAccountInfo(sockfd);
                sendAccountInfo(sockfd, account_number);
                break;
            case TRANSACT:
                account_number = readTransact(sockfd);
                sendBalance(sockfd, account_number);
                break;
            case GET_BALANCE:
                account_number = readGetBalance(sockfd);
                sendBalance(sockfd, account_number);
                break;
            case REQUEST_CASH:
                temp_amount = readRequestCash(sockfd);
                sendCash(sockfd, temp_amount);
                break;
            case ERROR:
                readError(sockfd);
                break;
            case TERMINATE:
                break;
        }//switch
        
    }//while not terminate

    free(arg);

    return;

}//func


int main(int argc, char *argv[]){
    if(argc != 4)
    {
        printSyntax();
        return 0;
    }

    // create empty output folder
    bookeepingCode();

    //CREATE EMPTY LOG THREAD
    // When logging, it
    // should iterate over each account in a global balance datastructure in a thread–safe manner (NOTE:
    // threads should be able to modify the balance immediately before and after an account is logged)
    // to perform logging of all account balances to the output file balances.csv with each line being
    // account number,balance,name,username,birthday in the format %d,%.2f,%s,%s,%ld\n
    pthread_t logtid;
    if(pthread_create(&logtid, NULL, doLogging, NULL) != 0) {
        printf("Log Thread failed to create\n");
    }
    printf("Log Thread created successfully\n");

    //initialize mutex for num_accounts global variable and the variable itself
    sem_init(&sem_num, 0, 1);
    num_accounts = 0;

    //CREATE SOCKET AND BEGIN LISTENING TO IT
    int sockfd, len;
    struct sockaddr_in servaddr, cli;
    

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 8)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        printf("Server listening..\n");
    len = sizeof(cli);

    //CREATE WORKER THREAD TO HANDLE CONNECTION
    //     For each incoming connection,
    // the server will create a worker thread which will handle the connection (pass it the connection’s file
    // descriptor) and return to listening on the socket. A worker thread will parse each query received
    // and reply with the appropriate response.
    pthread_t tid[10];
    int thread_idx = 0;    
    while(1) {
        // Accept the data packet from client and verification
        int *connfd = (int *) malloc(sizeof(int));
        *connfd = accept(sockfd, (SA *) &cli, &len);
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } else
            printf("Server accept the client...\n");

        if(pthread_create(&tid[thread_idx], NULL, func, (void *) connfd) != 0) {
            printf("Thread %d failed to create\n", thread_idx);
        }
        printf("Thread created successfully: %d\n", thread_idx);
        thread_idx++;
    }
    
    printf("Escaped while loop");
    // After chatting close the socket
    close(sockfd);

    return 0; 
}
