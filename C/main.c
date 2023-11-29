#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_TICKETS 10
#define NUM_FANS 12

int ticketsAvailable = MAX_TICKETS;
int nextFanInLine = 1;  

pthread_mutex_t ticketSalesMutex;
pthread_cond_t ticketSalesCondition;

void* purchaseTicket(void* arg) {
    int fanID = *((int*)arg);

    pthread_mutex_lock(&ticketSalesMutex);


    while (fanID != nextFanInLine) {
        pthread_cond_wait(&ticketSalesCondition, &ticketSalesMutex);
    }

    if (ticketsAvailable > 0) {
        printf("Fan %d is purchasing\n", fanID);
        printf("Tickets left: %d\n", ticketsAvailable);
        ticketsAvailable--;
        sleep(1); 
        printf("Fan: %d, Purchase Done\n", fanID);
        nextFanInLine++; 
    } else {
        printf("Fan %d can not purchase\n", fanID);
        printf("Tickets left: %d\n", ticketsAvailable);
        printf("Fan: %d, Purchase failed\n", fanID);
    }

    pthread_cond_broadcast(&ticketSalesCondition); 
    pthread_mutex_unlock(&ticketSalesMutex);

    return NULL;
}

int main() {
    pthread_t fans[NUM_FANS];
    int fanIDs[NUM_FANS];

    pthread_mutex_init(&ticketSalesMutex, NULL);
    pthread_cond_init(&ticketSalesCondition, NULL);

    for (int i = 0; i < NUM_FANS; i++) {
        fanIDs[i] = i + 1;
        if (pthread_create(&fans[i], NULL, &purchaseTicket, &fanIDs[i]) != 0) {
            perror("Failed to create fan thread");
        }
    }

    for (int i = 0; i < NUM_FANS; i++) {
        pthread_join(fans[i], NULL);
    }

    pthread_mutex_destroy(&ticketSalesMutex);
    pthread_cond_destroy(&ticketSalesCondition);

    return 0;
}
