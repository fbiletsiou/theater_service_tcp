#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <strings.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <unistd.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

#include "2114033_2113043_proj1.h"



int main(int argc, char *argv[] ){
    int sockfd, connfd; 
    socklen_t len;
    struct sockaddr_in servaddr, cli;
    int i;
    int available_teleph = N_tel;
    int available_cashiers = N_cash;

    unsigned int seed;
    
    //command line check
    if (argc < 1 || argc >= 2)  
    { 
        printf("enter a single argument eg.\"server.c \"\n"); 
        return 0; 
    }
    //======socket=====
    // socket creation and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("[-] Socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("[+] Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("[-] Socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("[+] Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("[-] Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("[-] Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("[-] Server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("[+] Server acccept the client...\n"); 
   
  
    // After chatting close the socket 
    close(sockfd); 


    //=================
    

    return 0;
}
