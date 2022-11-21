#include "logic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "test.h"
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
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

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    // (Wait for mutex to be unlocked) and lock it, so two processes/threads don't change stuff at the same time
    if (DEBUG) { printf("[%lu] ", pthread_self()); }

    if (DEBUG) {
        printf("> Waiting to perform balance change...\n");
    }
    else {
        printf("> Please wait...\n");
    }

    if ((status = pthread_mutex_lock(account_mutex)) != OK) {
        return status;
    }
    if (DEBUG) { printf("[%lu] ", pthread_self()); }

    if (DEBUG) {
        printf("> Done waiting to perform balance change!\n");
    }
    else {
        printf("> Transferring...\n");
    }

    // Sleep so other processes can get "queued"
    if (DEBUG) printf("[%lu] Sleeping to have other threads wait to see the effect...\n", pthread_self());
    // Get random sleep
    int randSleep;
    if ((status = randNum(1, SLEEP_MAX_MULTIPLICATION, &randSleep)) != OK) {
        if ((status = pthread_mutex_unlock(account_mutex)) != OK) {
            return status;
        }
        return status;
    }
    usleep(randSleep * BASE_SLEEP_MICRO_SECONDS);
    if (DEBUG) printf("[%lu] Done sleeping!\n", pthread_self());

    // Perform the balance change
    if (!TESTING) {
        if (DEBUG) printf("[%lu] > Changing balance by %d$...\n", pthread_self(), amount);
    }

    // First we get the current balance
    if ((status = getAccountBalance(balance)) != OK) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Couldn't get account balance! - %d\n", status);
        free(balance);
        if ((status = pthread_mutex_unlock(account_mutex)) != OK) {
            return status;
        }
        return status;
    }

    // We store the account balance before changing it, in case we crash during the change
    *commit_balance = *balance;

    // And then we change the balance
    if ((status = setAccountBalance(*balance + amount)) != OK) {
        if (DEBUG) { printf("[%lu] ", pthread_self()); }
        printf("Couldn't set balance! - %d\n", status);
        free(balance);
        if ((status = pthread_mutex_unlock(account_mutex)) != OK) {
            return status;
        }
        return status;
    }

    // Now that we're done, we unlock the mutex, so others can get to change stuff
    if ((status = pthread_mutex_unlock(account_mutex)) != OK) {
        return status;
    }
    if (DEBUG) { printf("[%lu] ", pthread_self()); }
    if (!TESTING) {
        printf("> Successfully transferred %d$!\n", amount);
    } else {
        printf("> Released lock.\n");
    }

    return status;
}

void *balanceCheck(void *balance_ptr) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *balance = (int *) balance_ptr;
    if ((*ret = pthread_mutex_lock(account_mutex)) != OK) {
        pthread_exit(ret);
    }
    if ((*ret = getAccountBalance(balance)) != OK) {
        pthread_exit(ret);
    }
    if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
        pthread_exit(ret);
    }
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

// we don't wait for this task to join, so the return value is kind of a "memory leak", but also not, since it's alive
// till the program dies.
void *houseKeepingTask() {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    while (true) {
        // to check if the mutex is in a deadlock, we have to try to lock it
        // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-pthread-mutex-trylock-attempt-lock-mutex-object
        if ((*ret = pthread_mutex_trylock(account_mutex)) == OK) {
            if (DEBUG) printf("Mutex got locked during housekeeping, trying to unlock\n");
            // if not locked, we locked it. Unlock it again
            if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
                pthread_exit(ret);
            }
        }
        else {
            // We could not lock the mutex - check the error
            switch (errno) {
                case EAGAIN: {
                    // maximum number of recursive locks for mutex has been exceeded
                    // this should not happen, so we unlock it to try and save the program, and reset the balance.
                    if ((*ret = setAccountBalance(*commit_balance)) != OK) {
                        // we don't return, since we still want to unlock the mutex, but an error occurred
                        printf("Error occurred while resetting balance from deadlocked mutex. - %d\n", *ret);
                    }
                    // reset commit balance
                    *commit_balance = 0;

                    if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
                        printf("Could not unlock mutex during housekeeping!\n");
                        pthread_exit(ret);
                    }
                    break;
                }
                case EBUSY: {
                    // mutex already locked
                    // wait to see if it gets unlocked, try every 200 microseconds
                    bool unlocked = false;
                    for (int i = 0; i < (HOUSEKEEPING_WAIT_FOR_UNLOCK_SECONDS*1000000)/200; i++) {
                        // try lock
                        if ((*ret = pthread_mutex_trylock(account_mutex)) == OK) {
                            if (DEBUG) printf("Mutex got locked during housekeeping, trying to unlock\n");
                            // it got unlocked and we locked it. Unlock it again
                            if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
                                pthread_exit(ret);
                            }
                            unlocked = true;
                            break;
                        }
                        usleep(200);
                    }

                    if (unlocked == false) {
                        if (DEBUG) printf("Housekeeping found deadlock - trying to unlock!\n");
                        
                        // if still locked, we forcibly unlock it, since we assume that a deadlock happened
                        if ((*ret = setAccountBalance(*commit_balance)) != OK) {
                            // we don't return, since we still want to unlock the mutex, but an error occurred
                            printf("Error occurred while resetting balance from deadlocked mutex. - %d\n", *ret);
                        }
                        // reset commit balance
                        *commit_balance = 0;

                        if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
                            printf("Could not unlock mutex during housekeeping!\n");
                            pthread_exit(ret);
                        }
                    }
                    break;
                }
                case EINVAL: {
                    printf("Housekeeping failure - unknown mutex!\n");
                    pthread_exit(ret);
                }
                default: {
                    printf("Housekeeping failure - unknown error: %s (%d) - ret: %d\n", strerror(errno), errno, *ret);
                    pthread_exit(ret);
                }
            }
        }
        sleep(HOUSEKEEPING_INTERVAL_SECONDS);
    }
    pthread_exit(ret);
}
