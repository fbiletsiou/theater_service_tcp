#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "2114033_2113043_proj1.h"


void* telephonist_func(void *arg);
void* cashier_func(void *arg);

unsigned int seed;
int main(int argc, char *argv[] ){
    int i;
    //command line check
    if (argc < 1 || argc >= 2)  
    { 
        printf("enter a single argument eg.\"server.c \"\n"); 
        return 0; 
    }

    //creation of telephonist threads
    pthread_t tid_telephonists[N_tel];
    int err;
    // Let us create the threads 
    for (i = 0; i < N_tel; i++){
        int *id = malloc(sizeof(*id));
        *id = i+1;
        err = pthread_create(&tid_telephonists[i], NULL, &telephonist_func,(void *) id);
        if(err !=0){
            perror("pthread_create() error");
            exit(1);
       }
    }
    //creation of cashier threads
    pthread_t tid_cashiers[N_cash];
    // Let us create the threads 
    for (i = 0; i < N_cash; i++){
        int *id = malloc(sizeof(*id));
        *id = i+1;
        err = pthread_create(&tid_cashiers[i], NULL, &cashier_func,(void *) id);
        if(err !=0){
            perror("pthread_create() error");
            exit(1);
       }
    }

    for(i = 0; i< N_tel; i++){
        pthread_join(tid_telephonists[i],NULL);
    }
    for(i = 0; i< N_cash; i++){
        pthread_join(tid_cashiers[i],NULL);
    }


    return 0;
}

void* telephonist_func(void *arg) {
    int myid = *((int *) arg);
    free(arg);
    
    printf("[T %d] Hey there\n",myid);
    return NULL;
}

void* cashier_func(void *arg) {
    int myid = *((int *) arg);
    free(arg);
    
    printf("[C %d] Hey there\n",myid);
    return NULL;
}