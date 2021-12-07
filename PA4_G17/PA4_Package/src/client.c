#include "client.h"


//given code
void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./client input_filename server_addr server_port\n");
}

//COPIED FROM RECITATION 11 CODE
void func(int sockfd) {
    char *buffer = "Hi Server, What is IP address for google.com?"; //Change or take out

    // TODO: send string (*buffer) to the server
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        perror("Cannot write");
        exit(3);
    }
    printf("%s\n", buffer);

    char recv[1024];
    memset(recv, 0, sizeof(recv));
    // TODO: receive message (IP address for google.com) from the server
    if (read(sockfd, recv, sizeof(recv)) < 0) {
        perror("cannot read");
        exit(4);
    }
    printf("Google.com IP address: %s\n", recv); //Take out
}

int main(int argc, char *argv[]){
    // argument handling + given code
    if(argc != 4)
    {
        printSyntax();
        return 0;
    }

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

