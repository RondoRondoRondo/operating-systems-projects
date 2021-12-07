#include "../include/client.h"

//copied from lab11
#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9001795
#define SA struct sockaddr

//given code
void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./client input_filename server_addr server_port\n");
}

//COPIED FROM RECITATION 11 CODE
void func(int sockfd) {

    //write all enums
    msg_enum mt = REGISTER;
    while(mt != TERMINATE){
        if (write(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("Cannot write");
            exit(3);
        }
        mt++;
    }
    //Write final TERMINATE enum
    if (write(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("Cannot write");
            exit(3);
    }
    
    //loop until receiving TERMINATE enum
    mt = REGISTER;
    while(mt != TERMINATE){
        if (read(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("cannot read");
            exit(4);
        }
        printf("Enum received : %d\n", mt);
    }
}

int main(int argc, char *argv[]){
    // argument handling + given code
    // if(argc != 4)
    // {
    //     printSyntax();
    //     return 0;
    // }

    //COPIED FROM RECITATION 11 CODE
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
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

    // connect the client socket to server socket
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } else
        printf("Connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);

    return 0; 
}

