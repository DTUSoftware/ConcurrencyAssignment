#ifndef BANK_LOGIC_H
#define BANK_LOGIC_H

void *withdraw(void *arg);

void *deposit(void *arg);

int createAccountDB();

int setAccountBalance(int balance);

int getAccountBalance(int *balance);

#endif //BANK_LOGIC_H
