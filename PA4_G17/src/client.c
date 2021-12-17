#include "../include/client.h"
#include <time.h> // For recording time.

//copied from lab11
#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9001795
#define SA struct sockaddr
boolean isConnected = false; 

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
                if (write(sockfd, &username, sizeof(username)) < 0) { //char[] username (NULL-terminated)
                    perror("Cannot write");
                    exit(3);
                }
                if (write(sockfd, &name, sizeof(name)) < 0) { //char[] name (NULL-terminated)
                    perror("Cannot write");
                    exit(3);
                }
                if (write(sockfd, &birthday, sizeof(birthday)) < 0) { //time_t birthday
                perror("Cannot write");
                exit(3);
                }
                break;
            
            case GET_ACCOUNT_INFO:
                if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
                    perror("Cannot write");
                    exit(3);
                }
                break;
            
            case TRANSACT:
                if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
                    perror("Cannot write");
                    exit(3);
                }
                if (write(sockfd, &amount, sizeof(amount)) < 0) { //float amount
                    perror("Cannot write");
                    exit(3);
                }
                break;
            
            case GET_BALANCE:
                if (write(sockfd, &account_number, sizeof(account_number)) < 0) { //int account_number
                    perror("Cannot write");
                    exit(3);
                }
                break;

            case ACCOUNT_INFO:
                printf("ACCOUNT_INFO : %d\n", mt);
                break;

            case BALANCE:
                printf("BALANCE : %d\n", mt);
                break;

            case REQUEST_CASH:
                if (write(sockfd, &amount, sizeof(amount)) < 0) { //float amount
                    perror("Cannot write");
                    exit(3);
                }
                break;

            case CASH:
                printf("CASH : %d\n", mt);
                break;

            case ERROR:
                if (write(sockfd, &mt, sizeof(msg_enum)) < 0) { //int message_type (the enumerated value)
                    perror("Cannot write");
                    exit(3);
                }
                break;

            case TERMINATE:
                if (write(sockfd, &, sizeof()) < 0) { //TYPE NONE ... ADD SENDING AND SIZEOF VALUE
                    perror("Cannot write");
                    exit(3);
                }
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
