#include "../include/client.h"
#include <time.h> // For recording time.

//copied from lab11
#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9001795
#define SA struct sockaddr
boolean isConnected = false; 

// helper functions
/*struct message msg1;
msg1.account_number = atoi(tok);
strcpy(msg1.name, tok);
...
Once msg1 fields have been filled by the tokens:
(In case REGISTER):
Register(sockfd, msg1);
*/

void Register(int sockfd, struct message msg){
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
    if (write(sockfd, &birthday, sizeof(birthday)) < 0) { //time_t birthday
    perror("Cannot write");
    exit(3);
    }
}
void getAccountInfo(int sockfd, struct message){
    int account_number = msg.account_number;
    if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
        perror("Cannot write");
        exit(3);
    }
}
void transact(int sockfd, struct message){
    int account_number = msg.account_number;
    if (write(sockfd, &username, sizeof(username)) < 0) { //int account_number
        perror("Cannot write");
        exit(3);
    }
    float amount = msg.amount;
    if (write(sockfd, &amount, sizeof(amount)) < 0) { //float amount
        perror("Cannot write");
        exit(3);
    }
}
void getBalance(int sockfd, struct message){
    int account_number = msg.account_number;
    if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
        perror("Cannot write");
        exit(3);
    }
}
struct message readAccountInfo(int sockfd){
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
    struct message{
        char * username;
        char * name;
        time_t birthday;
    } 
    return message;
}
struct message readBalance(int sockfd){
    int account_number;
    memset(account_number, 0, sizeof(account_number));
    if (read(sockfd, &account_number, sizeof(account_number)) < 0) {
            perror("cannot read");
            exit(4);
    }
    float balance;
    memset(balance, 0, sizeof(balance));
    if (read(sockfd, &balance, sizeof(balance)) < 0) {
            perror("cannot read");
            exit(4);
    }
    return balance;
}
void requestCash(int sockfd, float amount){
    float amount;
    if (write(sockfd, &amount, sizeof(amount)) < 0) { //float amount
        perror("Cannot write");
        exit(3);
    }
}
void readCash(int sockfd){
    float cash = msg.cash;
    if (read(sockfd, &cash, sizeof(cash)) < 0) { //float cash
        perror("Cannot write");
        exit(3);
    }
}
void sendError(int sockfd, struct message){
    msg_enum mt = msg.message; 
    if (write(sockfd, &mt, sizeof(mt)) < 0) { //float amount
        perror("Cannot write");
        exit(3);
    }
}
void Terminate(int sockfd, struct message){
    //ExpResp: NONE
    //DataValues: NONE
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

    char str[chunkSize];
    strcpy(str,line);
    char *rest = str;
    char *tok = strtok_r(line,",",&rest);

    //write all enums
    msg_enum mt = atoi(tok);

    // message struct
    tok = strtok_r(NULL,",",&rest);
    int account_number = atoi(tok);
    tok = strtok_r(NULL,",",&rest);
    char name[64] = tok;
    tok = strtok_r(NULL,",",&rest);
    char username[64] = tok;
    tok = strtok_r(NULL,",",&rest);
    time_t birthday = tok;
    tok = strtok_r(NULL,",",&rest);
    float amount = atof(tok);
    tok = strtok_r(NULL,",",&rest);
    int num_transactions = atoi(tok);


    //loop until receiving TERMINATE enum
    mt = REGISTER;
    while(mt != TERMINATE){
        if (read(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("cannot read");
            exit(4);
        }
        switch (mt){
            case REGISTER:
                Register(sockfd, msg1);
                struct message temp = readAccountInfo(sockfd);
                break;
            
            case GET_ACCOUNT_INFO:
                getAccountInfo(sockfd, msg1);
                //TODO
                break;
            
            case TRANSACT:
                transact(sockfd, msg1);
                //TODO
                break;
            
            case GET_BALANCE:
                getBalance(sockfd, msg1);
                //TODO
                break;

            case ACCOUNT_INFO:
                readAccountInfo(sockfd, msg1);
                //TODO
                break;

            case BALANCE:
                readBalance(sockfd, msg1);
                //TODO
                break;

            case REQUEST_CASH:
                requestCash(sockfd, msg1);
                //TODO
                break;

            case CASH:
                readCash(sockfd, msg1);
                //TODO
                break;

            case ERROR:
                sendError(sockfd, msg1);
                //TODO
                break;

            case TERMINATE:
                Terminate(sockfd, msg1);
                //TODO
                break;
        }//switch
    }//while
}//func

int main(int argc, char *argv[]){
    argument handling + given code
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
    File * fp;
    char line[chunksize];
    char *x = line;
    size_t len = chunksize;
    ssize_t read;

    fp = fopen(file, "r");
    if (fp == NULL) {
        fprintf(stderr,"Error opening file: %s \n",file);
        return NULL;
    }
    
    //TODO: Loop to send queries to the server. 
    while(getline(&x, &len, fp) != -1){ //until EOF
        //specifier: "%d,%d,%s,%s,%ld,%f,%d\n"
        //TODO: Serialize data and send to server.
        func(sockfd, x);
        //TODO: End connection after submitting TERMINATE message. // MOVE TO FUNC??
        close(sockfd); 
        //TODO: IF new messages, reconnect to server.
        if (isConnected == false) {
            if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
            printf("Connection with the server failed...\n");
            exit(0);
            } else {
                printf("Connected to the server..\n"); }
        }
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
