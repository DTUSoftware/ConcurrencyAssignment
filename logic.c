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
        if (readInput(bufferptr, 1, file) != OK) {
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
    } else {
        printf("> Please wait...\n");
    }

    if ((status = lockMutex()) != OK) {
        return status;
    }
    if (DEBUG) { printf("[%lu] ", pthread_self()); }

    if (DEBUG) {
        printf("> Done waiting to perform balance change!\n");
    } else {
        printf("> Transferring...\n");
    }

    // Sleep so other processes can get "queued"
    if (DEBUG) printf("[%lu] Sleeping to have other threads wait to see the effect...\n", pthread_self());
    // Get random sleep
    int randSleep;
    if ((status = randNum(1, SLEEP_MAX_MULTIPLICATION, &randSleep)) != OK) {
        if (DEBUG) printf("[%lu] Randnum failed: %d\n", pthread_self(), status);
        if ((status = unlockMutex()) != OK) {
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
        if ((status = unlockMutex()) != OK) {
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
        if ((status = unlockMutex()) != OK) {
            return status;
        }
        return status;
    }

    // Now that we're done, we unlock the mutex, so others can get to change stuff
    if ((status = unlockMutex()) != OK) {
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
    if ((*ret = lockMutex()) != OK) {
        pthread_exit(ret);
    }
    if ((*ret = getAccountBalance(balance)) != OK) {
        pthread_exit(ret);
    }
    if ((*ret = unlockMutex()) != OK) {
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

int lockMutex() {
    int status = OK;

    int randomNumber;
    if ((status = randNum(1, 696969, &randomNumber)) != OK) {
        printf("Could not generate a random number for mutex ID\n");
        return status;
    }

    if ((status = pthread_mutex_lock(account_mutex)) != OK) {
        printf("Could not lock mutex!\n");
        return status;
    }

    // the processes shouldn't know the pid of each other
    *current_mutex_id = pthread_self() * randomNumber;

    return status;
}

int unlockMutex() {
    int status = OK;

    // reset commit balance
    *commit_balance = 0;
    // clear current mutex ID BEFORE we unlock the mutex
    *current_mutex_id = 0;

    if ((status = pthread_mutex_unlock(account_mutex)) != OK) {
        printf("Could not unlock mutex!\n");
        return status;
    }

    return status;
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
            if (DEBUG)
                printf("[Housekeeping] Mutex not locked! Mutex got locked during housekeeping, trying to unlock\n");
            // if not locked, we locked it. Unlock it again
            if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
                printf("[Housekeeping] Could not unlock mutex during housekeeping!\n");
                pthread_exit(ret);
            } else {
                if (DEBUG) printf("[Housekeeping] Successfully unlocked again!\n");
            }
        } else {
            if (DEBUG) printf("[Housekeeping] Could not lock mutex, checking 'error'...\n");
            // We could not lock the mutex - check the error
            if (errno == EAGAIN || *ret == EAGAIN) {
                // maximum number of recursive locks for mutex has been exceeded
                // this should not happen, so we unlock it to try and save the program, and reset the balance.
                if ((*ret = setAccountBalance(*commit_balance)) != OK) {
                    // we don't return, since we still want to unlock the mutex, but an error occurred
                    printf("[Housekeeping] Error occurred while resetting balance from deadlocked mutex. - %d\n", *ret);
                }

                // reset and unlock
                if ((*ret = unlockMutex()) != OK) {
                    printf("[Housekeeping] Could not unlock mutex during housekeeping!\n");
                    pthread_exit(ret);
                } else {
                    if (DEBUG) printf("[Housekeeping] Successfully unlocked again!\n");
                }
            } else if (errno == EBUSY || *ret == EBUSY) {
                // mutex already locked
                // wait to see if it gets unlocked, try every 200 microseconds
                if (DEBUG) printf("[Housekeeping] Mutex already locked, checking to make sure it's not deadlocked.\n");
                bool unlocked = false;
                unsigned long locking_mutex_id = *current_mutex_id;
                for (int i = 0; i < HOUSEKEEPING_CHECKS_PER_SECOND * HOUSEKEEPING_WAIT_FOR_UNLOCK_SECONDS; i++) {
                    // try lock
                    // (it spams a LOT without the modulo for debugging, so we keep it to once every second)
                    if (DEBUG && ((i % 200) == 0))
                        printf("[Housekeeping] Are you still locked? (%d/%d)\n", i,
                               HOUSEKEEPING_CHECKS_PER_SECOND *
                               HOUSEKEEPING_WAIT_FOR_UNLOCK_SECONDS);

                    // check if it's even the same mutex now
                    if (locking_mutex_id != *current_mutex_id) {
                        if (DEBUG) printf("[Housekeeping] Different mutex, reset deadlock timer.\n");
                        unlocked = true;
                        break;
                    }

                    if ((*ret = pthread_mutex_trylock(account_mutex)) == OK) {
                        if (DEBUG)
                            printf("[Housekeeping] I am no longer locked! Mutex got locked during housekeeping, trying to unlock\n");
                        // it got unlocked and we locked it. Unlock it again
                        if ((*ret = pthread_mutex_unlock(account_mutex)) != OK) {
                            printf("[Housekeeping] Could not unlock mutex during housekeeping!\n");
                            pthread_exit(ret);
                        } else {
                            if (DEBUG) printf("[Housekeeping] Successfully unlocked again!\n");
                        }
                        unlocked = true;
                        break;
                    } else {
                        // (it spams a LOT without the modulo for debugging, so we keep it to once every second)
                        if (DEBUG && ((i % 200) == 0)) printf("[Housekeeping] Yes I am.\n");
                    }
                    usleep(1000000 / HOUSEKEEPING_CHECKS_PER_SECOND);
                }

                if (unlocked == false) {
                    if (DEBUG) printf("[Housekeeping] Housekeeping found deadlock - trying to unlock!\n");

                    // if still locked, we forcibly unlock it, since we assume that a deadlock happened
                    if ((*ret = setAccountBalance(*commit_balance)) != OK) {
                        // we don't return, since we still want to unlock the mutex, but an error occurred
                        printf("[Housekeeping] Error occurred while resetting balance from deadlocked mutex. - %d\n",
                               *ret);
                    } else {
                        if (DEBUG) printf("[Housekeeping] Successfully unlocked again!\n");
                    }

                    // reset and unlock
                    if ((*ret = unlockMutex()) != OK) {
                        printf("[Housekeeping] Could not unlock mutex during housekeeping!\n");
                        pthread_exit(ret);
                    }
                }
            } else if (errno == EINVAL || *ret == EINVAL) {
                printf("[Housekeeping] Housekeeping failure - unknown mutex!\n");
                pthread_exit(ret);
            } else {
                printf("[Housekeeping] Housekeeping failure - unknown error: %s (%d) - ret: %d\n", strerror(errno),
                       errno, *ret);
                pthread_exit(ret);
            }
        }
        sleep(HOUSEKEEPING_INTERVAL_SECONDS);
    }
    printf("[Housekeeping] Returning!?\n");
    pthread_exit(ret);
}
