#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "test.h"
#include "main.h"
#include "utils.h"

int testMain(int argc, char *argv[]) {
    int status = OK;
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
    else if (!strcmp(argv[1], "transfer"))
        if ((status = transferTest()) == OK)
            printf("Transfer test completed successfully!\n");
        else
            printf("Transfer test failed!\n");
    else {
        printf("Usage: bank [-test [menu | all | withdrawal | deposit | transfer]]\n");
        status = ERROR;
    }
    return status;
}

int testMenu() {
    clearScreen();
    printf("=====[ Testing Menu ]=====\n");
    printf("Choose Test:\n\t[0] Exit\n\t[1] Run All Tests\n\t[2] Withdrawal Test\n\t[3] Deposit Test\n\t[4] Transfer Test\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    int status = OK;

    switch (readChar) {
        case '0': {
            status = OK;
            break;
        }
        case '1': {
            if ((status = allTests()) == OK)
                printf("All tests completed successfully!\n");
            else
                printf("Tests failed!\n");
            break;
        }
        case '2': {
            if ((status = withdrawalTest()) == OK)
                printf("Withdrawal test completed successfully!\n");
            else
                printf("Withdrawal test failed!\n");
            break;
        }
        case '3': {
            if ((status = depositTest()) == OK)
                printf("Deposit test completed successfully!\n");
            else
                printf("Deposit test failed!\n");
            break;
        }
        case '4': {
            if ((status = transferTest()) == OK)
                printf("Transfer test completed successfully!\n");
            else
                printf("Transfer test failed!\n");
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
    return (withdrawalTest() && depositTest() && transferTest());
}

int withdrawalTest() {
    int status;

    pthread_t withdrawal_threads[MAX_THREAD_AMOUNT];
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

            // Withdraw a random amount for each thread
            for (int k = 0; k < i; k++) {
                // Get random number
                if ((status = randNum(1, 1000, &randInt)) != OK) {
                    return status;
                }
                // Multiply the random number by our current iteration
                int withdrawal_amount = k*randInt;
                // Subtract from our account balance
                account_balance = account_balance-withdrawal_amount;
                if (pthread_create(&withdrawal_threads[k], NULL, withdraw, (void *) &withdrawal_amount)) {
                    printf("error creating thread.");
                    return ERROR;
                }
            }

            // Join all the threads
            for (int k = 0; k < i; k++) {
                if (pthread_join(withdrawal_threads[k], &pthread_statuses[k])) {
                    printf("error joining thread.");
                    return ERROR;
                }
                if (pthread_statuses[k] == NULL) {
                    return ERROR;
                }
                if (*((int *) pthread_statuses[k]) != OK) {
                    free(pthread_statuses[k]);
                    return ERROR;
                }
                free(pthread_statuses[k]);
            }

            // Check account balance
            int *balance = malloc(sizeof(int));
            assert(balance != NULL);

            if (getAccountBalance(balance) != OK) {
                printf("Couldn't get account balance!\n");
                free(balance);
                return ERROR;
            }

            if (*balance != account_balance) {
                printf("Account balance not correct! (Expected %d - Actual %d)", account_balance, *balance);
                free(balance);
                return ERROR;
            }
            free(balance);
        }
    }

    return status;
}

int depositTest() {
    int status;
    if ((status = prepareBeforeTest()) != OK) {
        return status;
    }
    return status;
}

int transferTest() {
    int status;
    if ((status = prepareBeforeTest()) != OK) {
        return status;
    }
    return status;
}
