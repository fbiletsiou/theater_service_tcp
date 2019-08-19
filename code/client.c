#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <sys/socket.h> 
#include <netdb.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h>

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
#include "2114033_2113043_proj1.h"


void* client_func(void *arg);

unsigned int seed;
int main(int argc, char *argv[] ){
    int i;

    //command line check
    if (argc < 3 || argc >= 4)  
    { 
        printf("[-] Command line error \nEnter 3 arguments only eg.\"client.c number_of_customers rand_seed\"\n"); 
        return 0; 
    } 
    N_cust = atoi(argv[1]);
    seed = atoi(argv[2]);


  






    //creation of client threads
    pthread_t tid_clients[N_cust];
    int err;
    // Let us create the threads 
    for (i = 0; i < N_cust; i++){
        int *id = malloc(sizeof(*id));
        *id = i+1;
        err = pthread_create(&tid_clients[i], NULL, &client_func,(void *) id);
        if(err !=0){
            perror("pthread_create() error");
            exit(1);
       }
    }
    for(i = 0; i< N_cust; i++){
        pthread_join(tid_clients[i],NULL);
    }




    return 0;
}

void* client_func(void *arg) {
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    int myid = *((int *) arg);
    free(arg);
    
    printf("[C %d] Hey there\n",myid);
    //=====socket====
   // socket create and varification 
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
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("[-] Connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("[+] Connected to the server..\n"); 
    //===================   


    
    // close the socket 
    close(sockfd); 
    return NULL;
    
        /*
    //Thread created , time starts
    start_assisting = clock();


    //Setting timer for the waiting line
    struct sigaction temp;
    memset(&temp, 0, sizeof(temp));
    temp.sa_handler = timer_handler;
    sigfillset(&temp.sa_mask);
    sigdelset(&temp.sa_mask, SIGALRM);
    sigdelset(&temp.sa_mask, SIGINT );
    sigaction(SIGALRM, &temp, NULL);
    struct itimerval timer;

    timer.it_value.tv_sec =T_ihitiko;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec=T_ihitiko;
    timer.it_interval.tv_usec=0;


    // If there are no available telephonists , waiting for the condition
    while(available_teleph < 1){

        //Waiting Timer starts 
        if(setitimer (ITIMER_REAL, &timer, NULL)){
            perror("error calling setitimer()");
            exit(1);
        }

        
        //printf("Sorry custumer %d , there is no available telephonist to assist you at the moment \n ...WAITING...\n", myid);
        pthread_cond_wait(&cond_available_telephonist, &lock_available_telephonist); 
        end_waiting = clock(); // time that stoped waiting

        pthread_mutex_lock(&lock_av_time_waiting); //locking average time waiting counter
        T_av_waiting = T_av_waiting + cpu_time_used(start_assisting, end_waiting);
        pthread_mutex_unlock(&lock_av_time_waiting); //unlocking average time waiting counter

        printf("\n[WAITING LINE] :\t [%d available telephonists] client %d , we will assist you now\n\n",available_teleph,myid);
    }

    //There are available telephonists , so procceding     
    pthread_mutex_trylock(&lock_available_telephonist); //locking available telephonists
    available_teleph -=1;
    pthread_mutex_unlock(&lock_available_telephonist); //unlocking available telephonists
        
    // capacity pre-check for full theater
    //If it is full there is no reason for spending time
    if(plan.occupied_seats == N_seat){
        printf("ASSISTANT : \t Bad news client %d,your booking was canceled because the theater is full. \n\t We are really sorry \n\t\t Bye bye\n",myid);

        pthread_mutex_trylock(&lock_available_telephonist); //locking available telephonists
        available_teleph +=1;
        pthread_mutex_unlock(&lock_available_telephonist);//unlocking available telephonists 
        pthread_cond_signal(&cond_available_telephonist); //signaling condition waiting

        return NULL;
    }

    printf("ASSISTANT :\t Hello mrs/mr %d, how many tickets would you like?\n \t\t I would like to remind you that you can order %d - %d\n",myid,N_seat_min,N_seat_max);

    //Choosing the number of the tickets
    int N_cust_tickets =(rand_r(&seed)% (N_seat_max + 1 - N_seat_min)) + N_seat_min;
    printf("CLIENT %d:\t I would like to order %d tickets \n",myid,N_cust_tickets);
    printf("ASSISTANT : \t Excellent client %d, let me check the availability \n",myid);

    //Telephonist checking for availability
    sleep((rand_r(&seed)% (T_seat_max + 1 - T_seat_min)) + T_seat_min);

    if (plan.available_seats < N_cust_tickets){
        // Not enough 
        printf("ASSISTANT : \t Bad news client %d,your booking was canceled because there are not enough available places. \n\t\t We are really sorry \n\t\t Bye bye \n",myid);
    }
    else{
        // enough seats
        printf("ASSISTANT : \t Good news client %d, there are enough seats, I am proceeding to the checkout now\n" , myid);
        
        int temp_count = 0;
        //Reserving the seats
        printf("ASSISTANT : \t So mr/mrs %d, your %d seats will be the above ",myid,N_cust_tickets);

        pthread_mutex_lock(&lock_seats); //locking seats plan

        for (i=0; i<10; i++){
            for(int j=0; j<28; j++){
                if(plan.seat_plan[i][j] == 0){
                    plan.seat_plan[i][j] = myid; // Client Id
                    printf(" \t %d ",plan.occupied_seats);
                    plan.available_seats -= 1;
                    plan.occupied_seats += 1; 
                    temp_seat_no++;
                    temp_count++;  
                }
                if(temp_count == N_cust_tickets) break;
            }
            if(temp_count == N_cust_tickets) break; 
        }
        pthread_mutex_unlock(&lock_seats); //unlock seats plan

        // DEBUG 
        if(temp_count != N_cust_tickets){
            printf("ERROR: wrong reserving of the seats");
        }

        printf(" \n \t\t And the price is %d euro\n",(N_cust_tickets*C_seat));

        printf("ASSISTANT : \t okay client %d, we are now proceeding to the card payment\n", myid);
        // success propability 
        int P_card = (rand_r(&seed)% (100 + 1 - 1))+1;

        if(P_card >= 1 && P_card <=P_card_success){
            // Card payment success
            printf("ASSISTANT : \t Transaction Number %d\n",(N_transactions + 1));
            printf("ASSISTANT : \t Card payment successfully finished , thank you!\n");

            // Changing the company's balance
            pthread_mutex_lock(&lock_balance); //locking the company info
            balance = balance + (N_cust_tickets * C_seat);
            N_transactions += 1;
            pthread_mutex_unlock(&lock_balance); //unlock the company info

        } else{
            // Card payment failure
            printf("ASSISTANT : \t Bad news client %d,your booking was canceled because your card payment was not accepted\n\t\t Try again please. \n\t\t We are really sorry \n\t\t Bye bye \n",myid);
            printf("ASSISTANT : \t Releasing the seats \n");

            // Releasing the seats
            pthread_mutex_lock(&lock_seats); //locking seats plan

            for (i=0; i<10; i++){
                for(int j=0; j<28; j++){
                    if(plan.seat_plan[i][j] == myid){
                        plan.seat_plan[i][j] = 0; // Client Id
                        plan.available_seats += 1;
                        plan.occupied_seats -= 1; 
                        temp_seat_no--;
                    }
                }
            }
            pthread_mutex_unlock(&lock_seats); //unlock seats plan                

        } //if card payment closing
    } //if enough seats closing

    end_assisting = clock();
    pthread_mutex_lock(&lock_av_time_assisting); // locking average assisting time counter
    T_av_assisting = T_av_assisting + cpu_time_used(start_assisting, end_assisting);
    pthread_mutex_unlock(&lock_av_time_assisting); // unlocking average assisting time counter 

    pthread_mutex_trylock(&lock_available_telephonist); //lock available telephonists
    available_teleph +=1;
    pthread_mutex_unlock(&lock_available_telephonist); // unlock available telephonists
    pthread_cond_signal(&cond_available_telephonist); // signaling condition waiting
    return NULL;
*/
}