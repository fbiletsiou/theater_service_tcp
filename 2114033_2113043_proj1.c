/*NAMES
    Florina Biletsiou AM: 2114033
    Thomas Aggelopoulos AM: 2113043
*/
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "2114033_2113043_proj1.h"
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 
#include <signal.h> 


pthread_mutex_t lock_available_telephonist;
pthread_mutex_t lock_av_time_waiting;
pthread_mutex_t lock_av_time_assisting;
pthread_mutex_t lock_seats;
pthread_mutex_t lock_balance;


pthread_cond_t cond_available_telephonist=PTHREAD_COND_INITIALIZER;
     
clock_t start_assisting, end_assisting;
clock_t end_waiting;
double cpu_time_used(double s, double e);

unsigned int seed;
struct seats plan;

int temp_seat_no = 0;

double T_av_assisting = 0;
double T_av_waiting = 0;

void timer_handler (void)
{
  write(1,"\n\n[WAITING LINE] All our assistants are busy at the moment,we apologize. Please wait on the line\n\n",strlen("\n\n[WAITING LINE] All our assistants are busy at the moment,we apologize. Please wait on the line\n\n"));
}

void* client_func(void *arg) {
    //Thread created , time starts
    start_assisting = clock();

    int i;
    int myid = *((int *) arg);
    free(arg);

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

}

int main(int argc, char *argv[] ){

    int i,j = 0;

    //command line check
    if (argc < 3 || argc >= 4)  
    { 
        printf("enter 3 arguments only eg.\"*proj1.c number_of_customers rand_seed\""); 
        return 0; 
    } 
    N_cust = atoi(argv[1]);
    seed = atoi(argv[2]);


   //init of seats plan 
    for (i=0; i<10; i++){
        for(j=0; j<28; j++){
            plan.seat_plan[i][j] = 0; // 0--> FREE 
        }
    }
    plan.available_seats = N_seat;
    plan.occupied_seats = 0;

    //Mutex for available telephonists counter
    if (pthread_mutex_init(&lock_available_telephonist, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    
    // Mutex for average time waiting 
    if (pthread_mutex_init(&lock_av_time_waiting, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    // Mutex for average time assisting 
    if (pthread_mutex_init(&lock_av_time_assisting, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    // Mutex for seats plan 
    if (pthread_mutex_init(&lock_seats, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    //creation of client threads
    pthread_t tid[N_cust];
    int err;
    // Let us create the threads 
    for (i = 0; i < N_cust; i++){
        int *id = malloc(sizeof(*id));
        *id = i+1;
        err = pthread_create(&tid[i], NULL, &client_func,(void *) id);
        if(err !=0){
            perror("pthread_create() error");
            exit(1);
       }
    }
    for(i = 0; i< N_cust; i++){
        pthread_join(tid[i],NULL);
    }

    //Printing Results
    printf("~~THEATER PLAN~~\n");

    for (i=0; i<10; i++){
       for(j=0; j<28; j++){
           printf("%d\t", plan.seat_plan[i][j]); 
       }
       printf("\n");
    }
    printf("Detailed list:\n");
      for (i=0; i<10; i++){
        for(j=0; j<28; j++){
            printf("Seat %d : Client %d \n",(i*28)+j+1,plan.seat_plan[i][j]); 
        }
    }

    printf("Balance : %d euro\n Transactions: %d\n",balance,N_transactions);
    if(plan.occupied_seats == N_seat){
        printf("Theater FULL \n");
    }
    else{
        printf("Available %d/%d seats\n",plan.available_seats,N_seat);
    }

    printf("Average Time Waiting %f . \n",(double)T_av_waiting/(double)N_cust);
    printf("Average Time Assisting %f . \n",(double)T_av_assisting/(double)N_cust);

    //Destroying the mutexes
    pthread_mutex_destroy(&lock_available_telephonist);
    pthread_mutex_destroy(&lock_av_time_waiting);
    pthread_mutex_destroy(&lock_av_time_assisting);
    pthread_mutex_destroy(&lock_seats);
    pthread_mutex_destroy(&lock_balance);
    
    //Destroying Condition variable
    pthread_cond_destroy(&cond_available_telephonist); 

    pthread_exit(0);
}

double cpu_time_used(double s, double e){
    return ((double) (e - s)) / CLOCKS_PER_SEC;
}