#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "test.h"
#include "main.h"
#include "utils.h"
#include "logic.h"

bool TESTING = false;

int testMain(int argc, char *argv[]) {
    int status = OK;
    TESTING = true;
    if (argc < 2 || !strcmp(argv[1], "menu"))
        status = testMenu();
    else if (!strcmp(argv[1], "all"))
        if ((status = allTests()) == OK)
            printf("All tests completed successfully!\n");
        else
            printf("Tests failed!\n");
    else if (!strcmp(argv[1], "withdrawal"))
        if ((status = withdrawalTest()) == OK)
            printf("Withdrawal test completed successfully!\n");
        else
            printf("Withdrawal test failed!\n");
    else if (!strcmp(argv[1], "deposit"))
        if ((status = depositTest()) == OK)
            printf("Deposit test completed successfully!\n");
        else
            printf("Deposit test failed!\n");
    else if (!strcmp(argv[1], "deadlock"))
        if ((status = deadlockTest()) == OK)
            printf("Deadlock test completed successfully!\n");
        else
            printf("Deadlock test failed!\n");
    else {
        printf("Usage: bank [-test [menu | all | withdrawal | deposit | deadlock]]\n");
        status = ERROR;
    }
    return status;
}

int testMenu() {
    int status = OK;

    int *option = malloc(sizeof(int));
    assert(option != NULL);

    char *title = "DTU Student Bank ATM (Testing Menu)";
    char *optionText = "Choose Test";
    char *options[6];
    options[0] = "Exit";
    options[1] = "Run All Tests";
    options[2] = "Withdrawal Test";
    options[3] = "Deposit Test";
    options[4] = "Deadlock Test";
    options[5] = NULL;

    if ((status = menu(title, NULL, optionText, options, 1, option)) != OK) {
        free(option);
        return status;
    }

    switch (*option) {
        case 0: {
            status = OK;
            break;
        }
        case 1: {
            if ((status = allTests()) == OK)
                printf("All tests completed successfully!\n");
            else
                printf("Tests failed!\n");
            break;
        }
        case 2: {
            if ((status = withdrawalTest()) == OK)
                printf("Withdrawal test completed successfully!\n");
            else
                printf("Withdrawal test failed!\n");
            break;
        }
        case 3: {
            if ((status = depositTest()) == OK)
                printf("Deposit test completed successfully!\n");
            else
                printf("Deposit test failed!\n");
            break;
        }
        case 4: {
            if ((status = deadlockTest()) == OK)
                printf("Deadlock test completed successfully!\n");
            else
                printf("Deadlock test failed!\n");
            break;
        }
        default: {
            status = ERROR;
            break;
        }
    }
    return status;
}

int prepareBeforeTest() {
    int status;

    // Initialize the balance to 0
    if ((status = setAccountBalance(0)) != OK) {
        return status;
    }

    // Check that the balance is 0
    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if ((status = getAccountBalance(balance)) != OK) {
        return status;
    }

    if (*balance != 0) {
        printf("Failed to initialize account balance to 0!\n");
        status = ERROR;
    }

    return status;
}

int allTests() {
    if (withdrawalTest() == OK && depositTest() == OK && deadlockTest() == OK) {
        return OK;
    }
    return ERROR;
}

int runTest(int test_type) {
    int status;

    pthread_t threads[MAX_THREAD_AMOUNT];
    int amounts[MAX_THREAD_AMOUNT];
    void *pthread_statuses[MAX_THREAD_AMOUNT];

    // Random tests, running two or more pthreads at once
    for (int i = 2; i < MAX_THREAD_AMOUNT; i++) {
        // Prepare before each test
        if ((status = prepareBeforeTest()) != OK) {
            return status;
        }

        // Set totals
        int account_balance = 0;
        int randInt = 0;

        // Run x amount of random tests
        for (int j = 0; j < RANDOM_TESTS_PER_RUN; j++) {
            printf("Running test %d/%d with %d/%d threads.\n", j, RANDOM_TESTS_PER_RUN, i, MAX_THREAD_AMOUNT);

            // Withdraw a random amount for each thread
            for (int k = 0; k < i; k++) {
                // Get random number
                if ((status = randNum(1, 1000, &randInt)) != OK) {
                    return status;
                }
                // Multiply the random number by our current iteration
                amounts[k] = j * randInt;
                // Adjust our account balance
                switch (test_type) {
                    case WITHDRAWAL: {
                        account_balance = account_balance - amounts[k];
                        if ((status = pthread_create(&threads[k], NULL, withdraw, (void *) &amounts[k])) != OK) {
                            printf("error creating thread.");
                            return status;
                        }
                        break;
                    }
                    case DEPOSIT: {
                        account_balance = account_balance + amounts[k];
                        if ((status = pthread_create(&threads[k], NULL, deposit, (void *) &amounts[k])) != OK) {
                            printf("error creating thread.");
                            return status;
                        }
                        break;
                    }
                    default: {
                        printf("Unknown test type!\n");
                        return ERROR;
                    }
                }

            }

            // Join all the threads
            for (int k = 0; k < i; k++) {
                if ((status = pthread_join(threads[k], &pthread_statuses[k])) != OK) {
                    printf("error joining thread.");
                    return status;
                }
                if (pthread_statuses[k] == NULL) {
                    return ERROR;
                }
                status = *((int *) pthread_statuses[k]);
                // when using a value under or equal to 0, it should return ERROR
                if (j <= 0) {
                    if (status == OK) {
                        free(pthread_statuses[k]);
                        return status;
                    }
                } else {
                    if (status != OK) {
                        free(pthread_statuses[k]);
                        return status;
                    }
                }
                free(pthread_statuses[k]);
                status = OK;
            }

            // Check account balance
            int *balance = malloc(sizeof(int));
            assert(balance != NULL);

            if ((status = getAccountBalance(balance)) != OK) {
                printf("Couldn't get account balance!\n");
                free(balance);
                return status;
            }

            if (*balance != account_balance) {
                printf("Account balance not correct! (Expected %d - Actual %d)\n", account_balance, *balance);
                free(balance);
                return ERROR;
            }
            free(balance);
        }
    }

    return status;
}

int withdrawalTest() {
    printf("Running withdrawal tests...\n");
    return runTest(WITHDRAWAL);
}

int depositTest() {
    printf("Running deposit tests...\n");
    return runTest(DEPOSIT);
}

int deadlockTest() {
    int status = OK;
    printf("Running deadlock test...\n");

    // Prepare before test
    if ((status = prepareBeforeTest()) != OK) {
        return status;
    }

    // Set commit balance to 100
    *commit_balance = 100;

    printf("Locking mutex...\n");
    if ((status = pthread_mutex_lock(account_mutex)) != OK) {
        printf("Failed to lock mutex!\n");
        return status;
    }

    printf("Waiting for deadlock timeout... (%d seconds)\n", HOUSEKEEPING_WAIT_FOR_UNLOCK_SECONDS+5);
    sleep(HOUSEKEEPING_WAIT_FOR_UNLOCK_SECONDS+5); // add a buffer of 5 seconds
    printf("Done waiting - checking mutex status!\n");

    // check if it's still locked
    // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-pthread-mutex-trylock-attempt-lock-mutex-object
    if ((status = pthread_mutex_trylock(account_mutex)) == OK) {
        // if not locked, we locked it. Unlock it again
        if ((status = pthread_mutex_unlock(account_mutex)) != OK) {
            return status;
        }

        // check if the balance got set to 100
        int *balance = malloc(sizeof(int));
        assert(balance != NULL);

        if ((status = getAccountBalance(balance)) != OK) {
            printf("Couldn't get account balance!\n");
            free(balance);
            return status;
        }

        if (*balance != 100) {
            printf("Housekeeping did not restore balance to 100!\n");
            return ERROR;
        }
    }
    else {
        printf("Mutex is still locked, or some other error occurred!\n");
        return status;
    }

    return status;
}
