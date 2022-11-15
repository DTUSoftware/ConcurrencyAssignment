#include "logic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "test.h"
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "utils.h"

int createAccountDB() {
    return setAccountBalance(0); // initialize account balance to 0
}

// This performs IO and has to be called using Mutex from a function
int setAccountBalance(int balance) {
    FILE *file;
    file = fopen(ACCOUNT_DB, "w");
    if (file) {
        fprintf(file, "%d", balance);
        fclose(file);

        return OK;
    } else {
        return ERROR;
    }
}

// This performs IO and has to be called using Mutex from a function
int getAccountBalance(int *balance) {
    FILE *file;
    file = fopen(ACCOUNT_DB, "r");
    if (file) {
        char **bufferptr = malloc(sizeof(char *));
        assert(bufferptr != NULL);
        char *buffer = calloc(BUFFER_SIZE, sizeof(char));
        assert(buffer != NULL);
        bufferptr[0] = buffer;

        // Read input from console
        if (readinput(bufferptr, 1, file) != OK) {
            if (DEBUG) {
                printf("Couldn't read input!\n");
            }
            // if we could not read, and not caused by memory error, try again
            free(buffer);
            return 1;
        }
        buffer = *bufferptr;
        free(bufferptr);
        fclose(file);

//        if (DEBUG) {
//            printf("Account balance string: %s\n", buffer);
//        }

        if (convertStrToInt(buffer, balance) != OK) {
            if (DEBUG) {
                printf("Couldn't convert str to int!\n");
            }
            free(buffer);
            return ERROR;
        }

//        if (DEBUG) {
//            printf("Account balance int: %d\n", *balance);
//        }

        return OK;
    } else {
        if (DEBUG) {
            printf("File doesn't exist, trying to create file!\n");
        }
        *balance = 0;
        return createAccountDB();
    }
}

void *withdraw(void *arg) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *amount = (int *) arg;
    if (*amount <= 0) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Withdrawal amount cannot be below 0!\n");
        *ret = ERROR;
        pthread_exit(ret);
    }

    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    printf("> Waiting to withdraw...\n");
    pthread_mutex_lock(&account_mutex);
    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    printf("> Done waiting to withdraw...\n");

    if (DEBUG) {
        printf("[%lu] Sleeping to have other threads wait to see the effect...\n", pthread_self());
        fflush(stdout);
    }
    // Get random sleep
    int randSleep;
    if ((*ret = randNum(1, SLEEP_MAX_MULTIPLICATION, &randSleep)) != OK) {
        pthread_mutex_unlock(&account_mutex);
        pthread_exit(ret);
    }
    usleep(randSleep*BASE_SLEEP_MICRO_SECONDS);
    if (DEBUG) {
        printf("[%lu] Done sleeping!\n", pthread_self());
    }

    if (!TESTING) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("> Withdrawing %d$...\n", *amount);
    }

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if (getAccountBalance(balance) != OK) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Couldn't get account balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        *ret = ERROR;
        pthread_exit(ret);
    }

    if (setAccountBalance(*balance - *amount) != OK) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Couldn't set balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        *ret = ERROR;
        pthread_exit(ret);
    }

//    sleep(3);
    pthread_mutex_unlock(&account_mutex);
    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    if (!TESTING) {
        printf("> Done transferring!\n");
    }
    else {
        printf("> Released lock.\n");
    }
    *ret = OK;
    pthread_exit(ret);
}

void *deposit(void *arg) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *amount = (int *) arg;
    if (*amount <= 0) {
        printf("Deposit amount should be above 0.\n");
        *ret = ERROR;
        pthread_exit(ret);
    }

    printf("> Waiting to transfer...\n");
    pthread_mutex_lock(&account_mutex);
    printf("> Transferring %d$...\n", *amount);

    if (DEBUG) {
        printf("[%lu] Sleeping to have other threads wait to see the effect...\n", pthread_self());
        fflush(stdout);
    }
    // Get random sleep
    int randSleep;
    if ((*ret = randNum(1, SLEEP_MAX_MULTIPLICATION, &randSleep)) != OK) {
        pthread_mutex_unlock(&account_mutex);
        pthread_exit(ret);
    }
    usleep(randSleep*BASE_SLEEP_MICRO_SECONDS);
    if (DEBUG) {
        printf("[%lu] Done sleeping!\n", pthread_self());
    }

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if (getAccountBalance(balance) != OK) {
        printf("Couldn't get account balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        *ret = ERROR;
        pthread_exit(ret);
    }

    if (setAccountBalance(*balance + *amount) != OK) {
        printf("Couldn't set balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        *ret = ERROR;
        pthread_exit(ret);
    }

//    sleep(3);
    pthread_mutex_unlock(&account_mutex);
    printf("> Done transferring!\n");
    *ret = OK;
    pthread_exit(ret);
}
