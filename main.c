#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include "test.h"
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <fcntl.h>
#include <errno.h>
#include    <pthread.h>
#include "utils.h"
#include "logic.h"

bool DEBUG = true;
pthread_mutex_t *account_mutex = NULL;

int main(int argc, char *argv[]) {
    int status = OK;

    if ((status = init()) != OK) {
        printf("Failed to initialize!\n");
        return status;
    }

    // Run the functions
    if (argc < 2) {
        status = bankMenu();
    } else if (strcmp(argv[1], "-test") == 0)
        status = testMain(argc - 1, argv + 1);
    else {
        printf("Usage: bank [-test <menu | test [all | withdrawal | deposit | transfer]>]\n");
        status = ERROR;
    }
    return status;
}

int init() {
    int status = OK;

    // Set random seed to current time
    srand(time(NULL));

    // Shared Memory: https://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html
    // https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-shmat-shared-memory-attach-operation
    // https://man7.org/linux/man-pages/man2/shmget.2.html
    // https://www.ibm.com/docs/en/zos/2.1.0?topic=functions-shmget-get-shared-memory-segment
    // https://linux.die.net/man/2/shmat
    key_t key = SHARED_MEM_KEY; /* key to be passed to shmget() */
    int shmflg = IPC_CREAT | IPC_EXCL | 0777; /* shmflg to be passed to shmget() */
    int shmid; /* return value from shmget() */

    // Create segment
    printf("Creating segment with key %d\n", SHARED_MEM_KEY);
    if ((shmid = shmget(key, sizeof(int) + sizeof(pthread_mutex_t), shmflg)) == -1) {
        if (DEBUG) printf("Could not create segment - errno: %s (%d)\n", strerror(errno), errno);
        if (errno == EEXIST) {
            if (DEBUG) printf("Segment already created\n");
            // shmflg 0: try to get the created segment
            if ((shmid = shmget(key, sizeof(int) + sizeof(pthread_mutex_t), 0)) == -1) {
                if (DEBUG)
                    printf("Could not get already created shared mutex! - errno: %s (%d)\n", strerror(errno), errno);
                return shmid;
            } else {
                if (DEBUG) printf("Got created segment!\n");
            }
        } else {
            if (DEBUG) printf("Could not create shared mutex!\n");
            return shmid;
        }
    }

    // Attach segment to mutex lock
    void *shared_memory;
    if ((shared_memory = (void *) shmat(shmid, NULL, 0)) == (void *) -1) {
        if (DEBUG) printf("Could not attach segment\n");
        return ERROR;
    }

    int *initialized = shared_memory;

    account_mutex = shared_memory + sizeof(int);

    if (*initialized == -SHARED_MEM_INIT_KEY) {
        while (*initialized == -SHARED_MEM_INIT_KEY) {
            if (DEBUG) printf("Waiting for initialization to finish...\n");
            sleep(1);
        }
    } else if (*initialized != SHARED_MEM_INIT_KEY) {
        *initialized = -SHARED_MEM_INIT_KEY;
        if (DEBUG) printf("Initializing shared mutex...\n");

        // https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-pthread-mutex-init-initialize-mutex-object
        pthread_mutexattr_t attr;
        if ((status = pthread_mutexattr_init(&attr)) != OK) {
            return status;
        }

        // set mutex shared attribute
        if ((status = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) != OK) {
            return status;
        }

        // init mutex
        if ((status = pthread_mutex_init(account_mutex, &attr)) != OK) {
            return status;
        }

        if (DEBUG) printf("Initialized!\n");
        *initialized = SHARED_MEM_INIT_KEY;
    } else {
        if (DEBUG) printf("Got shared mutex!\n");
    }
    return status;
}

// Maximum of 10 options (0 - 9)
int menu(char *title, char *description, char *optionText, char **options, int columns, int *chosenOption) {
    int status = OK;
    *chosenOption = -1;

    clearScreen();

    printf("=====[ %s ]=====\n", title);
    if (description != NULL) {
        printf("%s\n\n", description);
    }

    if (optionText != NULL) {
        printf("%s:\n", optionText);
    } else {
        printf("Choose Option:\n");
    }

    int i = 0;
    char *option = options[i];
    while (option != NULL) {
        printf("\t[%d] %s", i, option);

        if (((i + 1) % columns) == 0) {
            printf("\n");
        }

        option = options[++i];
    }
    printf("\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    switch (readChar) {
        case '0': {
            *chosenOption = 0;
            break;
        }
        case '1': {
            *chosenOption = 1;
            break;
        }
        case '2': {
            *chosenOption = 2;
            break;
        }
        case '3': {
            *chosenOption = 3;
            break;
        }
        case '4': {
            *chosenOption = 4;
            break;
        }
        case '5': {
            *chosenOption = 5;
            break;
        }
        case '6': {
            *chosenOption = 6;
            break;
        }
        case '7': {
            *chosenOption = 7;
            break;
        }
        case '8': {
            *chosenOption = 8;
            break;
        }
        case '9': {
            *chosenOption = 9;
            break;
        }
        default: {
            *chosenOption = -1;
            status = ERROR;
            break;
        }
    }

    return status;
}

int bankMenu() {
    int status = OK;
    while (status == OK) {
        int *option = malloc(sizeof(int));
        assert(option != NULL);

        char *title = "DTU Student Bank ATM";
        char *description = "Welcome to YOUR bank for all your banking needs (blockchain support coming Fiscal Year 2069) :)";
        char *options[6];
        options[0] = "Exit";
        options[1] = "Withdraw";
        options[2] = "Deposit";
        options[3] = "Check account";
        options[4] = "Transfer";
        options[5] = NULL;

        if ((status = menu(title, description, NULL, options, 1, option)) != OK) {
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
                status = withdrawMenu();
                break;
            }
            case 2: {
                status = depositMenu();
                break;
            }
            case 3: {
                status = accountMenu();
                break;
            }
            case 4: {
                status = transferMenu();
                break;
            }
                // hidden test menu
            case 9: {
                printf("ENTERING TEST MENU");
                for (int i = 0; i < 5; i++) {
                    printf(".");
                    fflush(stdout);
                    usleep(800000);
                }
                status = testMenu();
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

int menuDoneWait() {
    printf("\n\nDo you want a receipt?\n\t[0] No\t[1] Yes\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    if (readChar == '1') {
        printf("\nPrinting receipt");
        fflush(stdout);
        for (int i = 0; i < 3; i++) {
            printf(".");
            fflush(stdout);
            sleep(1);
        }
        printf("\n");
    }

    return OK;
}

// Only to be called from other menus
int actionMenu(int action_type) {
    int status = OK;

    if (action_type != WITHDRAWAL && action_type != DEPOSIT && action_type != TRANSFER) {
        status = ERROR;
        return status;
    }

    int *option = malloc(sizeof(int));
    assert(option != NULL);

    char *title = (action_type == WITHDRAWAL) ? "Withdrawal Menu" : ((action_type == DEPOSIT) ? "Deposit Menu"
                                                                                              : "Transfer Menu");
    char *optionText = (action_type == WITHDRAWAL) ? "Choose Withdraw Amount" : ((action_type == DEPOSIT)
                                                                                 ? "Choose Deposit Amount"
                                                                                 : "Choose Transfer Amount");
    char *options[8];
    options[0] = "50kr,-";
    options[1] = "100kr,-";
    options[2] = "200kr,-";
    options[3] = "500kr,-";
    options[4] = "1000kr,-";
    options[5] = "Custom amount";
    options[6] = "Return";
    options[7] = NULL;

    if ((status = menu(title, NULL, optionText, options, 2, option)) != OK) {
        free(option);
        return status;
    }

    int *amount = malloc(sizeof(*amount));
    assert(amount != NULL);

    switch (*option) {
        case 0: {
            *amount = 50;
            break;
        }
        case 1: {
            *amount = 100;
            break;
        }
        case 2: {
            *amount = 200;
            break;
        }
        case 3: {
            *amount = 500;
            break;
        }
        case 4: {
            *amount = 1000;
            break;
        }
        case 5: {
            while (true) {
                if (getCustomValue(amount) != OK) {
                    free(amount);
                    return ERROR;
                }

                if (*amount > 0) {
                    break;
                }

                printf("Please enter an amount above 0\n\n");
            }

            break;
        }
        case 6: {
            return OK;
        }
        default: {
            free(amount);
            return ERROR;
        }
    }

    pthread_t thread;

    switch (action_type) {
        case TRANSFER:
        case WITHDRAWAL: {
            if (pthread_create(&thread, NULL, withdraw, (void *) amount)) {
                printf("Error creating thread.\n");
                free(amount);
                status = ERROR;
                return status;
            }
            break;
        }
        case DEPOSIT: {
            if (pthread_create(&thread, NULL, deposit, (void *) amount)) {
                printf("Error creating thread.\n");
                free(amount);
                status = ERROR;
                return status;
            }
            break;
        }
        default: {
            status = ERROR;
            return status;
        }
    }

    if (DEBUG) printf("Waiting for thread to join...\n");

    void *pthread_status = NULL;
    if (pthread_join(thread, &pthread_status)) {
        printf("Error joining thread.\n");
        free(amount);
        return ERROR;
    }
    free(amount);
    if (pthread_status == NULL) {
        return ERROR;
    }
    if (*((int *) pthread_status) != OK) {
        free(pthread_status);
        return ERROR;
    }
    free(pthread_status);

    menuDoneWait();
    return OK;
}

int withdrawMenu() {
    return actionMenu(WITHDRAWAL);
}

int depositMenu() {
    return actionMenu(DEPOSIT);
}

int transferMenu() {
    return actionMenu(TRANSFER);
}

int accountMenu() {
    int status = OK;

    clearScreen();
    printf("=====[ Account Menu ]=====\n\n");

    int *balance = malloc(sizeof(int));
    assert(balance != NULL);

    pthread_t thread;

    if (pthread_create(&thread, NULL, balanceCheck, (void *) balance)) {
        printf("Error creating thread.\n");
        free(balance);
        status = ERROR;
        return status;
    }

    if (DEBUG) printf("Waiting for thread to join...\n");

    void *pthread_status = NULL;
    if (pthread_join(thread, &pthread_status)) {
        printf("Error joining thread.\n");
        free(balance);
        return ERROR;
    }
    if (pthread_status == NULL) {
        return ERROR;
    }
    if (*((int *) pthread_status) != OK) {
        free(pthread_status);
        return ERROR;
    }
    free(pthread_status);

    printf("Your current balance is %d$!\n", *balance);

    free(balance);

    menuDoneWait();
    return status;
}
