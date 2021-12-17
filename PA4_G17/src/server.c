#include "../include/server.h"

#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9001795
#define SA struct sockaddr

int num_accounts;
void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

void doLogging(){
    //TODO: wait 5 seconds
    //TODO: iterate through the accounts array defined in server.h
    //TODO: print account number,balance,name,username,birthday to balances.csv using "%d,%.2f,%s,%s,%ld\n” as the format
    //notes: 1. the array is an array of structs. Said struct is defined in server.h as well. 2. The array is 1000+ long so don't iterate through all of it, stop after the last account
    //3. I made a global variable num_accounts so only iterate through that many accounts
}

// Function designed for chat between client and server.
void func(void *arg) {
    // cast arg to client socket (sockfd)
    int sockfd = *(int *) arg;
    
    int account_number, amount;
    char name[64];
    char username[64];
    float cash, balance;
    time_t birthday;

    // A worker thread will parse each query received and reply with the appropriate response.
    msg_enum mt = REGISTER;
    while(mt != TERMINATE){
        if (read(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("cannot read");
            exit(4);
        }
        

        //printf("Enum received: %d\n", mt);
        switch (mt){
            case REGISTER:
                sem_init(&semaphores[num_accounts], 0, 1);
                //TODO: read the next bytes into the above variables
                //TODO: if changing the balance, signal a yet-to-be-created condition variable for the log thread?
                sem_wait(&semaphores[account_number]);
                //TODO: initialize the struct at accounts[account_number] to the data we just received.
                sem_post(&semaphores[account_number]);
                //TODO: write back the appropriate return message, which for this is BALANCE
                num_accounts++; 
                break;
            case GET_ACCOUNT_INFO:
                printf("GET_ACCOUNT_INFO : %d\n", mt); 
                break;
            case TRANSACT:
                printf("TRANSACT : %d\n", mt); 
                break;
            case GET_BALANCE:
                printf("GET_BALANCE : %d\n", mt);
                break;
            case ACCOUNT_INFO:
                printf("ACCOUNT_INFO : %d\n", mt);
                break;
            case BALANCE:
                printf("BALANCE : %d\n", mt);
                break;
            case REQUEST_CASH:
                printf("REQUEST_CASH : %d\n", mt);
                break;
            case CASH:
                printf("CASH : %d\n", mt);
                break;
            case ERROR:
                printf("ERROR : %d\n", mt);
                break;
            case TERMINATE:
                printf("TERMINATE : %d\n", mt);
                break;
        }
        
        if (write(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("Cannot write");
            exit(3);
        }
    }

    printf("Everything received");
}


int main(int argc, char *argv[]){
    argument handling
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

    //CREATE SOCKET AND BEGIN LISTENING TO IT
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    num_accounts = 0;

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
    pthread_t tid;    
    while(1) {
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA *) &cli, &len);
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } else
            printf("Server accept the client...\n");

        if(pthread_create(&tid, NULL, func, (void *) &connfd) != 0) {
            printf("Thread %d failed to create\n", count);
        }
        printf("Thread created successfully: %d\n", count);
    }
    

    // After chatting close the socket
    close(sockfd);

    return 0; 
}
