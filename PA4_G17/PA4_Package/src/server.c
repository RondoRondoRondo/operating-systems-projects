#include "server.h"

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

// Function designed for chat between client and server.
void func(int sockfd) {
    char buffer[MAX];
    memset(buffer, 0, sizeof(buffer));
    // TODO: Read message sent by the client
    read(sockfd, buffer, sizeof(buffer));

    printf("Received a question \"%s\" from client.\n", buffer);

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%s", GOOGLEIPADDR);
    // TODO: Send IP address for google.com to the client
    write(sockfd, buffer, sizeof(buffer));

    printf("Answered \"%s\" to client.\n", buffer);
}


int main(int argc, char *argv[]){
    // argument handling
    if(argc != 4)
    {
        printSyntax();
        return 0;
    }

    // create empty output folder
    bookeepingCode();

    int sockfd, connfd, len;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *) &cli, &len);
    if (connfd < 0) {
        printf("Server accept failed...\n");
        exit(0);
    } else
        printf("Server accept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);

    return 0; 
}

