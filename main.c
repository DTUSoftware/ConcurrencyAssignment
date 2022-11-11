#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "test.h"
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

bool DEBUG = true;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return bankMenu();
    } else if (!strcmp(argv[1], "-test"))
        return testMain(argc - 1, argv + 1);
    else {
        printf("Usage: bank [-test <menu | test [all | withdrawal | deposit | transfer]>]\n");
        return 1;
    }
}

int bankMenu() {
    enum STATUS status = OK;
    while (status == OK) {
        clearScreen();
        printf("=====[ DTU Student Bank ATM ]=====\nWelcome to YOUR bank for all your banking needs (blockchain support coming Fiscal Year 2069) :)\n\n");
        int *option = malloc(sizeof(int));
        if (option == NULL) {
            status = ERROR;
            continue;
        }

        if (chooseOption(option) != 0) {
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
                if (withdrawMenu() != 0) status = ERROR;
                break;
            }
            case 2: {
                if (depositMenu() != 0) status = ERROR;
                break;
            }
            case 3: {
                if (accountMenu() != 0) status = ERROR;
                break;
            }
            case 4: {
                if (transferMenu() != 0) status = ERROR;
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
    FILE *file;
    file = fopen(ACCOUNT_DB, "w");

    if (file) {
        fputs("0", file); // initialize account balance to 0
        fclose(file);
        return 0;
    } else {
        return 1;
    }
}

// This performs IO and has to be called using Mutex from a function
int setAccountBalance(int balance) {
    FILE *file;
    file = fopen(ACCOUNT_DB, "w");
    if (file) {
        fprintf(file, "%d", balance); // initialize account balance to 0
        fclose(file);

        return 0;
    } else {
        return 1;
    }
}

// This performs IO and has to be called using Mutex from a function
int getAccountBalance(int *balance) {
    FILE *file;
    file = fopen(ACCOUNT_DB, "r");
    if (file) {
        char **bufferptr = malloc(sizeof(char *));
        if (bufferptr == NULL) {
            perror("out of memory");
            return 1;
        }
        char *buffer = calloc(BUFFER_SIZE, sizeof(char));
        if (buffer == NULL) {
            perror("out of memory");
            return 1;
        }
        bufferptr[0] = buffer;

        // Read input from console
        if (!readinput(bufferptr, 1, file)) {
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

        if (convertStrToInt(buffer, balance) != 0) {
            if (DEBUG) {
                printf("Couldn't convert str to int!\n");
            }
            free(buffer);
            return 1;
        }

        return 0;
    } else {
        if (DEBUG) {
            printf("Couldn't open file!\n");
        }
        *balance = 0;
        return createAccountDB();
    }
}

// Reads input from STDIN, while keeping a buffer, in order to
// read "unlimited" input
// IMPORTANT: From our Shell Assignment
int readinput(char **bufferptr, int newlinestop, FILE *stream) {
    char *buffer = *bufferptr;
    // We tried using getline(), but it wasn't in MingW, and stumbled upon this implementation that
    // could not really be altered - so credit where credit is due.
    // thanks to https://gist.github.com/btmills/4201660
    unsigned int buffer_size = BUFFER_SIZE;
    int ch = EOF;
    int pos = 0;

    while (!((ch = fgetc(stream)) == '\n' && newlinestop == 1) && ch != EOF && !feof(stream)) {
        buffer[pos++] = ch;
        if (pos == buffer_size) {
            buffer_size = buffer_size + BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size * sizeof(char));
            if (buffer == NULL) {
                perror("memory allocation");
                exit(EXIT_FAILURE);
            }
        }
    }
    buffer_size = ++pos;
    buffer = realloc(buffer, buffer_size + 1 * sizeof(char));
    if (buffer == NULL) {
        perror("memory allocation");
        exit(EXIT_FAILURE);
    }
    buffer[pos] = '\0';

//            printf("DEBUG: %s\n", buffer);
    bufferptr[0] = buffer;
    return 1;
}

int clearScreen() {
    // https://www.geeksforgeeks.org/clear-console-c-language/
    printf("\e[1;1H\e[2J");
    return 0;
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

    return 0;
}

void *withdraw(void *arg) {
    int *amount = (int *) arg;
    printf("> Waiting to transfer...\n");
    pthread_mutex_lock(&account_mutex);
    printf("> Transferring %d$...\n", *amount);

    int *balance = malloc(sizeof(int));
    if (getAccountBalance(balance) != 0) {
        printf("Couldn't get account balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        return NULL;
    }

    if (setAccountBalance(*balance - *amount) != 0) {
        printf("Couldn't set balance!\n");
        free(balance);
        pthread_mutex_unlock(&account_mutex);
        return NULL;
    }

    sleep(3);
    // TODO: Actually remove the money lol
    pthread_mutex_unlock(&account_mutex);
    printf("> Done transferring!\n");
    return NULL;
}

int withdrawMenu() {
    int *withdrawal_amount = malloc(sizeof(*withdrawal_amount));
    if (withdrawal_amount == NULL) {
        printf("Malloc failed\n");
        return 1;
    }

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
            if (getCustomValue(withdrawal_amount) != 0) {
                free(withdrawal_amount);
                return 1;
            }
            break;
        }
        case '6': {
            return 0;
        }
        default: {
            free(withdrawal_amount);
            return 1;
        }
    }

    pthread_t withdrawal_thread;

    if (pthread_create(&withdrawal_thread, NULL, withdraw, (void *) withdrawal_amount)) {
        printf("error creating thread.");
        free(withdrawal_amount);
        return 1;
    }
    if (DEBUG) printf("Waiting for thread to join...\n");
    if (pthread_join(withdrawal_thread, NULL)) {
        printf("error joining thread.");
        free(withdrawal_amount);
        return 1;
    }
    free(withdrawal_amount);

    menuDoneWait();
    return 0;
}

int depositMenu() {
    clearScreen();
    printf("=====[ Deposit Menu ]=====\n\n");
    printf("Choose Deposit Amount:\n\t[0] 50kr,-\t[1] 100kr,-\n\n\t[2] 200kr,-\t[3] 500kr,-\n\n\t[4] 1000kr,-\t[5] Custom amount\n\n\t[6] Return\n\n");


    return 0;
}

int accountMenu() {
    clearScreen();
    printf("=====[ Account Menu ]=====\n\n");

    // TODO: put me in a pthread

    int *balance = malloc(sizeof(int));

    if (getAccountBalance(balance) != 0) {
        printf("Couldn't get account balance!\n");
        free(balance);
        return 1;
    }

    printf("Your current balance is %d$!\n", *balance);

    free(balance);

    // TODO: wait for input instead
    sleep(3);
    return 0;
}

int transferMenu() {
    clearScreen();
    printf("=====[ Transfer Menu ]=====\n\n");

    printf("Choose Transfer Amount:\n\t[0] 50kr,-\t[1] 100kr,-\n\n\t[2] 200kr,-\t[3] 500kr,-\n\n\t[4] 1000kr,-\t[5] Custom amount\n\n[6] Return\n\n");

    return 0;
}

int convertStrToInt(char *string, int *integer) {
    // thanks stackoverflow https://stackoverflow.com/a/3068420
    if (strlen(string) > (floor(log10(abs(INT_MAX))) + 1)) {
        printf("You exceeded the maximum value of an integer!\n");
        return 1;
    }

    // TODO: check for integers over - strtol

    // Convert buffered string to integer
    *integer = atoi(string);
    return 0;
}

int getCustomValue(int *value) {
    printf("Write desired amount:\n");

    printf("> ");
    fflush(stdout);

    char **bufferptr = malloc(sizeof(char *));
    if (bufferptr == NULL) {
        perror("out of memory");
        return 1;
    }
    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        perror("out of memory");
        return 1;
    }
    bufferptr[0] = buffer;

    // Read input from console
    if (!readinput(bufferptr, 1, stdin)) {
        // if we could not read, and not caused by memory error, try again
        free(buffer);
        return 1;
    }
    buffer = *bufferptr;
    free(bufferptr);

    if (convertStrToInt(buffer, value) != 0) {
        free(buffer);
        return 1;
    }
    free(buffer);
    return 0;
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
            return 1;
        }
    }

    return 0;
}
