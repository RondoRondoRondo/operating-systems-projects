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


// HELPER FUNCTIONS
//  
//   
//    
//     

int readRegister(int sockfd){
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
    if (read(sockfd, &birthday, sizeof(birthday)) < 0) {
            perror("cannot read");
            exit(4);
    }

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
    int account_number = 0;
    if (read(sockfd, &account_number, sizeof(account_number)) < 0) {
            perror("cannot read");
            exit(4);
    }
    float amount = 0.0;
    if (read(sockfd, &amount, sizeof(amount)) < 0) {
            perror("cannot read");
            exit(4);
    }

    sem_wait(&semaphores[account_number]);
    accounts[account_number].balance += amount;
    sem_post(&semaphores[account_number]);

    return account_number;


}//readTransact


int readGetBalance(int sockfd){
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
    msg_enum mt = BALANCE;
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    if (write(sockfd, &account_num, sizeof(account_num)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    float bal = accounts[account_num].balance;
    if (write(sockfd, &bal, sizeof(bal)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    return;
}

float readRequestCash(int sockfd){
    float amount = 0.0;
    if (read(sockfd, &amount, sizeof(amount)) < 0) {
            perror("cannot read");
            exit(4);
    }

    return amount;
}//readRequestCash

void sendCash(int sockfd, float amount){
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


void readError(int sock){
    int message_type = 0;
    if (read(sockfd, &message_type, sizeof(message_type)) < 0) {
            perror("cannot read");
            exit(4);
    }
    printf("Error: message %d was received by server", message_type);
    return;
}//readError


// Function designed for chat between client and server.
void func(void *arg) {
    // cast arg to client socket (sockfd)
    int sockfd = *(int *) arg;
    
    int account_number;
    // char name[64];
    // char username[64];
    // float cash, balance;
    // time_t birthday;

    // parse each query received and reply with the appropriate response.
    msg_enum mt = REGISTER;
    while(mt != TERMINATE){
        if (read(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("cannot read");
            exit(4);
        }
        

        //printf("Enum received: %d\n", mt);
        switch (mt){
            case REGISTER:
                int account_number = readRegister(sockfd);
                sendBalance(sockfd, account_number);
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
        }//switch
        
    }//while not terminate

    free(arg);

}//func


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
            printf("Thread %d failed to create\n", count);
        }
        printf("Thread created successfully: %d\n", count);
        thread_idx++;
    }
    

    // After chatting close the socket
    close(sockfd);

    return 0; 
}
