#ifndef BANK_LOGIC_H
#define BANK_LOGIC_H

void *withdraw(void *amount_ptr);

void *deposit(void *amount_ptr);

void *balanceCheck(void *balance_ptr);

int createAccountDB();

int setAccountBalance(int balance);

int getAccountBalance(int *balance);

int changeBalance(int amount);

void *houseKeepingTask();

int lockMutex();

int unlockMutex();

#endif //BANK_LOGIC_H
