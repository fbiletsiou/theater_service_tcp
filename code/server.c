#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <strings.h>
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>  

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

#include "2114033_2113043_proj1.h"
#define TRUE   1  
#define FALSE  0  


int main(int argc, char *argv[] ){
    int sockfd; 
    struct sockaddr_in servaddr;
    int i,max_sd;
    char client_message[256];
    int available_teleph = N_tel;
    int available_cashiers = N_cash;
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[150] ,  
          max_clients = 150 , activity, valread , sd;

    unsigned int seed;
    
    //command line check
    if (argc < 1 || argc >= 2)  
    { 
        printf("enter a single argument eg.\"server.c \"\n"); 
        return 0; 
    }
    //======socket=====
    // socket creation and verification 
    
    //set of socket descriptors  
    fd_set readfds;
         
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = -1;   
    } 

    //master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
        printf("[-] Master Socket creation failed");
        exit(EXIT_FAILURE);
    }  
    else
        printf("[+] Socket successfully created..\n"); 
    //setting master socket to allow multiple connections ,  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,sizeof(opt)) < 0 ){   
        printf("[-] Setsockopt failed");   
        exit(EXIT_FAILURE);   
    }   
    //bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(master_socket, (SA*)&servaddr, sizeof(servaddr))) < 0) { 
        printf("[-] Socket bind failed...\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
        printf("[+] Socket successfully binded..\n");
    //accept the incoming connection  
    addrlen = sizeof(servaddr);   
    puts("[+] Waiting for connections ...\n");   
    
    while (TRUE){
        //clear the socket set  
        FD_ZERO(&readfds);
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;

        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
        //wait for an activity on one of the sockets , timeout is NULL ,  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR)){   
            printf("[-] Select error\n");   
        }

        //If something happened on the master socket then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds)){   
        if((new_socket = accept(master_socket,(struct sockaddr *)&servaddr, (socklen_t*)&addrlen))<0){
            printf("[-] Accept failed");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("[+] New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(servaddr.sin_addr) , servaddr.sin_port);
                 
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++){   
                //if position is empty  
                if( client_socket[i] == -1 ){   
                    client_socket[i] = new_socket;   
                    printf("[+]Adding to list of sockets as %d\n" , i);   
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++){   
            sd = client_socket[i];      
            if (FD_ISSET( sd , &readfds)){   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , client_message, 256)) == 0){   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&servaddr ,(socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(servaddr.sin_addr) , servaddr.sin_port);   
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = -1;   
                }   
                     
                // message that came in  
                else{   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    client_message[valread] = '\0';
                    printf("Received %s from %d\n",client_message,sd);
                }   
            }   
        }


    //===========

    }
    
    
   
  
    // After chatting close the socket 
    close(sockfd); 


    //=================
    

    return 0;
}
