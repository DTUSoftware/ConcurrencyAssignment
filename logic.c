#include "logic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "test.h"
#include <unistd.h>
#include <pthread.h>
#include "utils.h"

// Creates an account with a balance of 0
// should be called using Mutex, since it performs IO
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

int changeBalance(int amount) {
    int status = OK;

    // (Wait for mutex to be unlocked) and lock it, so two processes/threads don't change stuff at the same time
    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    printf("> Waiting to perform balance change...\n");
    pthread_mutex_lock(&account_mutex);
    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    printf("> Done waiting to perform balance change!\n");

    // Sleep so other processes can get "queued"
    if (DEBUG) {
        printf("[%lu] Sleeping to have other threads wait to see the effect...\n", pthread_self());
        fflush(stdout);
    }
    // Get random sleep
    int randSleep;
    if ((status = randNum(1, SLEEP_MAX_MULTIPLICATION, &randSleep)) != OK) {
        pthread_mutex_unlock(&account_mutex);
        return status;
    }
    usleep(randSleep * BASE_SLEEP_MICRO_SECONDS);
    if (DEBUG) {
        printf("[%lu] Done sleeping!\n", pthread_self());
    }

    // Perform the balance change
    if (!TESTING) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("> Changing balance by %d$...\n", amount);
    }

    // First we get the current balance
    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if ((status = getAccountBalance(balance)) != OK) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Couldn't get account balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        return status;
    }

    // And then we change the balance
    if ((status = setAccountBalance(*balance + amount)) != OK) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Couldn't set balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        return status;
    }

    // Now that we're done, we unlock the mutex, so others can get to change stuff
//    pthread_mutex_unlock(&account_mutex);
    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    if (!TESTING) {
        printf("> Done transferring!\n");
    } else {
        printf("> Released lock.\n");
    }

    return status;
}

void *balanceCheck(void *balance_ptr) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *balance = (int *) balance_ptr;
    *ret = getAccountBalance(balance);
    pthread_exit(ret);
}

void *withdraw(void *amount_ptr) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *amount = (int *) amount_ptr;
    if (*amount <= 0) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Withdrawal amount cannot be below 0!\n");
        *ret = ERROR;
        pthread_exit(ret);
    }

    *ret = changeBalance(-*amount);
    pthread_exit(ret);
}

void *deposit(void *amount_ptr) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *amount = (int *) amount_ptr;
    if (*amount <= 0) {
        printf("Deposit amount should be above 0.\n");
        *ret = ERROR;
        pthread_exit(ret);
    }

    *ret = changeBalance(*amount);
    pthread_exit(ret);
}
