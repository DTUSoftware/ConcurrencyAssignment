#ifndef BANK_UTILS_H
#define BANK_UTILS_H

#include <stdio.h>

int clearScreen();

int getCustomValue(int *value);

int readInput(char **bufferptr, int newlinestop, FILE *stream);

int menuDoneWait();

int convertStrToInt(char *string, int *integer);

int randNum(int lower, int upper, int *randNum);

#endif //BANK_UTILS_H
