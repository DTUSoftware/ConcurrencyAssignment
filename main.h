#ifndef BANK_MAIN_H
#define BANK_MAIN_H

#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define BUFFER_SIZE 20
#define BASE_SLEEP_MICRO_SECONDS 1000000
#define SLEEP_MAX_MULTIPLICATION 5
#define SHARED_MEM_KEY 6967
#define SHARED_MEM_INIT_KEY 42069
#define HOUSEKEEPING_INTERVAL_SECONDS 1
#define HOUSEKEEPING_WAIT_FOR_UNLOCK_SECONDS 15
#define ACCOUNT_DB "./account_db"

int main(int argc, char *argv[]);

int init();

int menu(char *title, char *description, char *optionText, char **options, int columns, int *chosenOption);

int bankMenu();

int actionMenu(int action_type);

int withdrawMenu();

int depositMenu();

int accountMenu();

extern bool DEBUG;

extern pthread_mutex_t *account_mutex;
extern int *commit_balance;

enum STATUS {
    OK = 0,
    DEAD = -1,
    ERROR = 1
};

enum ACTION_TYPE {
    WITHDRAWAL,
    DEPOSIT
};

#endif //BANK_MAIN_H
