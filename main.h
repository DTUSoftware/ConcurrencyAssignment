#ifndef BANK_MAIN_H
#define BANK_MAIN_H

#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>

#define BUFFER_SIZE 20
#define ACCOUNT_DB "./account_db"

int main();

int chooseOption(int *optionChosen);

int withdrawMenu();

int depositMenu();

int accountMenu();

int transferMenu();

int clearScreen();

int getCustomValue(int *value);

int readinput(char **bufferptr, int newlinestop, FILE *stream);

int menuDoneWait();

void *withdraw(void *arg);

int createAccountDB();

int getAccountBalance(int *balance);

int convertStrToInt(char *string, int *integer);

bool DEBUG = true;


pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

enum STATUS {
    OK = 1,
    DEAD = 0,
    ERROR = -1
};

#endif //BANK_MAIN_H
