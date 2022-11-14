#ifndef BANK_MAIN_H
#define BANK_MAIN_H

#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define BUFFER_SIZE 20
#define ACCOUNT_DB "./account_db"

int main(int argc, char *argv[]);

int bankMenu();

int chooseOption(int *optionChosen);

int withdrawMenu();

int depositMenu();

int accountMenu();

int transferMenu();

void *withdraw(void *arg);

void *deposit(void *arg);

int createAccountDB();

int setAccountBalance(int balance);

int getAccountBalance(int *balance);

extern bool DEBUG;

extern pthread_mutex_t account_mutex;

enum STATUS {
    OK = 0,
    DEAD = -1,
    ERROR = 1
};

#endif //BANK_MAIN_H
