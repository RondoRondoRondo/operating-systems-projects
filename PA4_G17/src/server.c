#include "../include/server.h"

#define LOCALHOST "127.0.0.1"
#define MAX 80
#define PORT 9001795
#define SA struct sockaddr

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

// Function designed for chat between client and server.
void func(int sockfd) {
    
    msg_enum mt = REGISTER;
    while(mt != TERMINATE){
        if (read(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("cannot read");
            exit(4);
        }
        
        printf("Enum received: %d\n", mt);
        
        if (write(sockfd, &mt, sizeof(msg_enum)) < 0) {
            perror("Cannot write");
            exit(3);
        }
    }

    printf("Everything received");
}


int main(int argc, char *argv[]){
    // argument handling
    // if(argc != 4)
    // {
    //     printSyntax();
    //     return 0;
    // }

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
    servaddr.sin_addr.s_addr = htonl(LOCALHOST);
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

