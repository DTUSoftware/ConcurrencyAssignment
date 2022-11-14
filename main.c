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

bool DEBUG = true;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return bankMenu();
    } else if (strcmp(argv[1], "-test") == 0)
        return testMain(argc - 1, argv + 1);
    else {
        printf("Usage: bank [-test <menu | test [all | withdrawal | deposit | transfer]>]\n");
        return ERROR;
    }
} //dab dab dab dab

int bankMenu() {
    enum STATUS status = OK;
    while (status == OK) {
        clearScreen();
        printf("=====[ DTU Student Bank ATM ]=====\nWelcome to YOUR bank for all your banking needs (blockchain support coming Fiscal Year 2069) :)\n\n");
        int *option = malloc(sizeof(int));
        assert(option != NULL);

        if (chooseOption(option) != OK) {
            status = ERROR;
            free(option);
            continue;
        }

        // If exit was chosen
        if (*option == 0) {
            status = DEAD;
            free(option);
            continue;
        }

        // Switch on any other option
        switch (*option) {
            case 1: {
                if (withdrawMenu() != OK) status = ERROR;
                break;
            }
            case 2: {
                if (depositMenu() != OK) status = ERROR;
                break;
            }
            case 3: {
                if (accountMenu() != OK) status = ERROR;
                break;
            }
            case 4: {
                if (transferMenu() != OK) status = ERROR;
                break;
            }
            default: {
                break;
            }
        }

        free(option);
    }

    return 0;
}

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

        if (convertStrToInt(buffer, balance) != OK) {
            if (DEBUG) {
                printf("Couldn't convert str to int!\n");
            }
            free(buffer);
            return ERROR;
        }

        return OK;
    } else {
        if (DEBUG) {
            printf("Couldn't open file!\n");
        }
        *balance = 0;
        return createAccountDB();
    }
}

int menuDoneWait() {
    printf("Do you want a receipt?\n\t[0] No\t[1] Yes\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    if (readChar == '1') {
        printf("Printing receipt...\n");
        sleep(3);
    }

    return OK;
}

void *withdraw(void *arg) {
    int *ret = (int *) malloc(sizeof(int));
    assert(ret != NULL);

    int *amount = (int *) arg;
    if (*amount <= 0) {
        printf("Withdrawal amount cannot be below 0!\n");
        *ret = ERROR;
        pthread_exit(ret);
    }

    printf("> Waiting to withdraw...\n");
    pthread_mutex_lock(&account_mutex);
    printf("> Withdrawing %d$...\n", *amount);

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if (getAccountBalance(balance) != OK) {
        printf("Couldn't get account balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        *ret = ERROR;
        pthread_exit(ret);
    }

    if (setAccountBalance(*balance - *amount) != OK) {
        printf("Couldn't set balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        *ret = ERROR;
        pthread_exit(ret);
    }

    sleep(3);
    pthread_mutex_unlock(&account_mutex);
    printf("> Done transferring!\n");
    *ret = OK;
    pthread_exit(ret);
}

int withdrawMenu() {
    int *withdrawal_amount = malloc(sizeof(*withdrawal_amount));
    assert(withdrawal_amount != NULL);

    clearScreen();
    printf("=====[ Withdrawal Menu ]=====\n");
    printf("Choose Withdraw Amount:\n\t[0] 50kr,-\t[1] 100kr,-\n\n\t[2] 200kr,-\t[3] 500kr,-\n\n\t[4] 1000kr,-\t[5] Custom amount\n\n\t[6] Return\n\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    switch (readChar) {
        case '0': {
            *withdrawal_amount = 50;
            break;
        }
        case '1': {
            *withdrawal_amount = 100;
            break;
        }
        case '2': {
            *withdrawal_amount = 200;
            break;
        }
        case '3': {
            *withdrawal_amount = 500;
            break;
        }
        case '4': {
            *withdrawal_amount = 1000;
            break;
        }
        case '5': {
            while (true) {
                if (getCustomValue(withdrawal_amount) != OK) {
                    free(withdrawal_amount);
                    return ERROR;
                }

                if (*withdrawal_amount > 0) {
                    break;
                }

                printf("Please enter an amount above 0 \n\n");
            }

            break;
        }
        case '6': {
            return OK;
        }
        default: {
            free(withdrawal_amount);
            return ERROR;
        }
    }

    pthread_t withdrawal_thread;

    if (pthread_create(&withdrawal_thread, NULL, withdraw, (void *) withdrawal_amount)) {
        printf("error creating thread.");
        free(withdrawal_amount);
        return ERROR;
    }
    if (DEBUG) printf("Waiting for thread to join...\n");
    void *pthread_status;
    if (pthread_join(withdrawal_thread, &pthread_status)) {
        printf("error joining thread.");
        free(withdrawal_amount);
        return ERROR;
    }
    free(withdrawal_amount);
    if (*((int *) pthread_status) != OK) {
        free(pthread_status);
        return ERROR;
    }
    free(pthread_status);

    menuDoneWait();
    return OK;
}

void *deposit(void *arg) {
    int *amount = (int *) arg;

    if (*amount <= 0) {
        printf("Deposit amount should be above 0.\n");
        pthread_exit((void *) ERROR);
    }

    printf("> Waiting to transfer...\n");
    pthread_mutex_lock(&account_mutex);
    printf("> Transferring %d$...\n", *amount);

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if (getAccountBalance(balance) != OK) {
        printf("Couldn't get account balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        pthread_exit((void *) ERROR);
    }

    if (setAccountBalance(*balance + *amount) != OK) {
        printf("Couldn't set balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        pthread_exit((void *) ERROR);
    }

    sleep(3);
    pthread_mutex_unlock(&account_mutex);
    printf("> Done transferring!\n");
    pthread_exit((void *) OK);
}

int depositMenu() {
    int *deposit_amount = malloc(sizeof(*deposit_amount));
    assert(deposit_amount != NULL);

    clearScreen();
    printf("=====[ Deposit Menu ]=====\n\n");
    printf("Choose Deposit Amount:\n\t[0] 50kr,-\t[1] 100kr,-\n\n\t[2] 200kr,-\t[3] 500kr,-\n\n\t[4] 1000kr,-\t[5] Custom amount\n\n\t[6] Return\n\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    switch (readChar) {
        case '0': {
            *deposit_amount = 50;
            break;
        }
        case '1': {
            *deposit_amount = 100;
            break;
        }
        case '2': {
            *deposit_amount = 200;
            break;
        }
        case '3': {
            *deposit_amount = 500;
            break;
        }
        case '4': {
            *deposit_amount = 1000;
            break;
        }
        case '5': {
            while (true) {
                if (getCustomValue(deposit_amount) != OK) {
                    free(deposit_amount);
                    return ERROR;
                }

                if (*deposit_amount > 0) {
                    break;
                }

                printf("You can only deposit an amount above 0.\n");
            }

            break;
        }
        case '6': {
            return OK;
        }
        default: {
            free(deposit_amount);
            return ERROR;
        }
    }
    pthread_t deposit_thread;

    if (pthread_create(&deposit_thread, NULL, deposit, (void *) deposit_amount)) {
        printf("error creating thread.");
        free(deposit_amount);
        return ERROR;
    }
    if (DEBUG) printf("Waiting for thread to join...\n");

    void *pthread_status = NULL;
    if (pthread_join(deposit_thread, &pthread_status)) {
        printf("error joining thread.");
        free(deposit_amount);
        return ERROR;
    }
    free(deposit_amount);
    if (*((int *) pthread_status) != OK) {
        return ERROR;
    }

    menuDoneWait();

    return OK;
}

int accountMenu() {
    clearScreen();
    printf("=====[ Account Menu ]=====\n\n");

    // TODO: put me in a pthread

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    if (getAccountBalance(balance) != OK) {
        printf("Couldn't get account balance!\n");
        free(balance);
        return ERROR;
    }

    printf("Your current balance is %d$!\n", *balance);

    free(balance);

    // TODO: wait for input instead
    sleep(3);
    return OK;
}

int transferMenu() {
    clearScreen();
    int *transfer_amount = malloc(sizeof(*transfer_amount));

    printf("=====[ Transfer Menu ]=====\n\n");

    printf("Choose Transfer Amount:\n\t[0] 50kr,-\t[1] 100kr,-\n\n\t[2] 200kr,-\t[3] 500kr,-\n\n\t[4] 1000kr,-\t[5] Custom amount\n\n[6] Return\n\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    switch (readChar) {
        case '0': {
            *transfer_amount = 50;
            break;
        }
        case '1': {
            *transfer_amount = 100;
            break;
        }
        case '2': {
            *transfer_amount = 200;
            break;
        }
        case '3': {
            *transfer_amount = 500;
            break;
        }
        case '4': {
            *transfer_amount = 1000;
            break;
        }
        case '5': {
            while (true) {
                if (getCustomValue(transfer_amount) != OK) {
                    free(transfer_amount);
                    return ERROR;
                }

                if (*transfer_amount > 0) {
                    break;
                }

                printf("You can only transfer an amount above 0.\n");
            }

            break;
        }
        case '6': {
            return OK;
        }
        default: {
            free(transfer_amount);
            return ERROR;
        }
    }

    printf("Enter reciveing account nr\n\n> ");
/*
    int Reciver_account = 0;

    getCustomValue(Reciver_account);
*/
    pthread_t transfer_thread;

    if (pthread_create(&transfer_thread, NULL, withdraw, (void *) transfer_amount)) {
        printf("error creating thread.");
        free(transfer_amount);
        return ERROR;
    }
    if (DEBUG) printf("Waiting for thread to join...\n");

    void *pthread_status = NULL;
    if (pthread_join(transfer_thread, &pthread_status)) {
        printf("error joining thread.");
        free(transfer_amount);
        return ERROR;
    }
    free(transfer_amount);
    if (*((int *) pthread_status) != OK) {
        return ERROR;
    }

    menuDoneWait();


    return OK;
}

int chooseOption(int *optionChosen) {
    printf("Choose Option:\n\t[0] Exit\n\t[1] Withdraw\n\t[2] Deposit\n\t[3] Check account\n\t[4] Transfer\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    switch (readChar) {
        case '0': {
            *optionChosen = 0;
            break;
        }
        case '1': {
            *optionChosen = 1;
            break;
        }
        case '2': {
            *optionChosen = 2;
            break;
        }
        case '3': {
            *optionChosen = 3;
            break;
        }
        case '4': {
            *optionChosen = 4;
            break;
        }
        case '5': {
            *optionChosen = 5;
            break;
        }
        default: {
            *optionChosen = -1;
            return ERROR;
        }
    }

    return OK;
}
