#include "../include/client.h"
#include <time.h> // For recording time.

//copied from lab11
#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9796
#define SA struct sockaddr
bool isConnected = false; 
float theCash = 0.0;

// helper functions
void Register(int sockfd, struct message msg){
    if (write(sockfd, &msg.msg_type, sizeof(msg.msg_type)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    char username[64];
    strcpy(username, msg.username);
    if (write(sockfd, &username, sizeof(username)) < 0) { //char[] username (NULL-terminated)
        perror("Cannot write");
        exit(3);
    }
    char name[64];
    strcpy(name, msg.name);
    if (write(sockfd, &name, sizeof(name)) < 0) { //char[] name (NULL-terminated)
        perror("Cannot write");
        exit(3);
    }
    time_t birthday = msg.birthday;
    if (write(sockfd, &birthday, 8) < 0) { //time_t birthday
    perror("Cannot write");
    exit(3);
    }
    return;
}
void getAccountInfo(int sockfd, struct message msg){
    if (write(sockfd, &msg.msg_type, sizeof(msg.msg_type)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    int account_number = msg.acc_num;
    if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
        perror("Cannot write");
        exit(3);
    }
    return;
}
void transact(int sockfd, struct message msg){
    //printf("Sending TRANSACT \n");
    if (write(sockfd, &msg.msg_type, sizeof(msg.msg_type)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    //printf("In transact: wrote message type %d, \n", msg.msg_type);
    int account_number = msg.acc_num;
    if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
        perror("Cannot write");
        exit(3);
    }
    //printf("In transact: wrote account_number %d, \n", account_number);
    float amount = msg.cash;
    if (write(sockfd, &amount, sizeof(amount)) < 0) { //float amount
        perror("Cannot write");
        exit(3);
    }
    //printf("In transact: wrote amount%.2f, \n", amount);
    theCash += msg.cash;
    return;
}
void getBalance(int sockfd, struct message msg){
    msg_enum mttt = GET_BALANCE;
    if (write(sockfd, &mttt, sizeof(mttt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    int account_number = msg.acc_num;
    if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
        perror("Cannot write");
        exit(3);
    }
    return;
}
struct message readAccountInfo(int sockfd){
    msg_enum temp = 0;
    if (read(sockfd, &temp, sizeof(temp)) < 0) { //first four bytes read
        perror("Cannot read");
        exit(4);
    }
    char username[64];
    memset(username, 0, sizeof(username));
    if (read(sockfd, &username, sizeof(username)) < 0) {
            perror("cannot read");
            exit(4);
    }
    char name[64];
    memset(name, 0, sizeof(name));
    if (read(sockfd, &name, sizeof(name)) < 0) {
            perror("cannot read");
            exit(4);
    }
    time_t birthday;
    if (read(sockfd, &birthday, sizeof(birthday)) < 0) {
            perror("cannot read");
            exit(4);
    }
    struct message msgt;
    msgt.msg_type = temp;
    strcpy(msgt.username,username);
    strcpy(msgt.name,name);
    msgt.birthday = birthday;
    return msgt;
}
float readBalance(int sockfd){
    msg_enum temp = 0;
    if (read(sockfd, &temp, sizeof(temp)) < 0) { //first four bytes read
        perror("Cannot read");
        exit(4);
    }
    int account_number = 0;
    if (read(sockfd, &account_number, sizeof(account_number)) < 0) {
            perror("cannot read");
            exit(4);
    }
    float balance = 0.0;
    if (read(sockfd, &balance, sizeof(balance)) < 0) {
            perror("cannot read");
            exit(4);
    }
    return balance;
}
void requestCash(int sockfd, float amount){
    msg_enum temp = REQUEST_CASH;
    if (write(sockfd, &temp, sizeof(temp)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    if (write(sockfd, &amount, sizeof(amount)) < 0) { //float amount
        perror("Cannot write");
        exit(3);
    }
    return;
}
void readCash(int sockfd){
    msg_enum temp = 0;
    if (read(sockfd, &temp, sizeof(temp)) < 0) { //first four bytes read
        perror("Cannot read");
        exit(4);
    }
    float cash = 0.0;
    if (read(sockfd, &cash, sizeof(cash)) < 0) { //float cash
        perror("Cannot write");
        exit(3);
    }
    theCash += cash;
    return;
}
void sendError(int sockfd, struct message msg){
    msg_enum mt = ERROR;
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    //printf("writing error num: %d to server\n", msg.msg_type);
    if (write(sockfd, &msg.msg_type, sizeof(msg.msg_type)) < 0) { //message_type mt
        perror("Cannot write");
        exit(3);
    }
    return;
}
void Terminate(int sockfd){
    msg_enum mt = TERMINATE;
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //first four bytes send
        perror("Cannot write");
        exit(3);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////Rest of Code Below/////////////////////////////
//////////////////////////////////////////////////////////////////////////

//given code
void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./client input_filename server_addr server_port\n");
}

//COPIED FROM RECITATION 11 CODE
void func(int sockfd, char * line) {

    char *tok = strtok(line,",");

    // message struct
    struct message msg1;
    msg1.msg_type = atoi(tok);
    tok = strtok(NULL,",");
    msg1.acc_num = atoi(tok);
    tok = strtok(NULL,",");
    strcpy(msg1.name, tok);
    tok = strtok(NULL,",");
    strcpy(msg1.username, tok);
    tok = strtok(NULL,",");
    msg1.birthday = atol(tok);
    //printf("Bday: %ld\n", msg1.birthday);
    tok = strtok(NULL,",");
    msg1.cash = atof(tok);
    tok = strtok(NULL,",");
    msg1.num_tx = atoi(tok);
    
    //swtich cases
    switch (msg1.msg_type){
        case REGISTER:
            //printf("Sending Register \n");
            Register(sockfd, msg1);
            float temp_bal = readBalance(sockfd);
            //printf("Received balance: %.2f\n", temp_bal);
            break;

        case TRANSACT:
            //printf("case TRANSACT \n");
            getBalance(sockfd, msg1);
            float tempy = readBalance(sockfd);
            //printf("Received balance: %.2f \n", tempy);
            if (tempy + msg1.cash >= 0) {
                if (theCash + msg1.cash < 0) {
                    //printf("Asking for cash \n");
                    requestCash(sockfd, msg1.cash);
                    readCash(sockfd);
                }
                transact(sockfd, msg1);
                readBalance(sockfd);
            }
            break;

        case TERMINATE:
            Terminate(sockfd);
            //printf("DISCONNECTING\n");
            isConnected = false; 
            close(sockfd);
            break;

        default:
            sendError(sockfd, msg1);
            break;

    }//switch   
}//func

int main(int argc, char *argv[]){
    if(argc != 4) {
        printSyntax();
        return 0;
    }

    //COPIED FROM RECITATION 11 CODE
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verifications
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n"); }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servaddr.sin_port = htons(PORT);

    // TODO: Start record time.
    clock_t begin = clock();
    // connect the client socket to server socket
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } else {
        printf("Connected to the server..\n"); }

    isConnected = true;

    //TODO: Read file containing queries to the server.
    FILE * fp;
    char line[200];
    char *x = line;
    size_t len = 200;
    ssize_t read;
    char path[50];
    sprintf(path,"input/%s", argv[1]);

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr,"Error opening file: %s \n", path);
    }
    
    //TODO: Loop to send queries to the server. 
    while(getline(&x, &len, fp) != -1){ //until EOF
        //TODO: IF not connected && new messages, reconnect to server.
        if (!isConnected) {
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1) {
                printf("Socket recreation failed...\n");
                exit(0);
            } else {
                printf("Socket successfully recreated..\n"); }
            if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
            printf("Connection with the server failed...\n");
            exit(0);
            } else {
                printf("Connected to the server..\n"); }
            isConnected = true;
        }
        func(sockfd, x);
    }
    fclose(fp);

    //TODO: End record time.
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Ellapsed Time: %.2f\n", time_spent);

    // close the socket
    close(sockfd);

    return 0; 
}
