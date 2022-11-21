#ifndef BANK_TEST_H
#define BANK_TEST_H

#include <stdbool.h>

#define MAX_THREAD_AMOUNT 10
#define RANDOM_TESTS_PER_RUN 2

extern bool TESTING;

int testMain(int argc, char *argv[]);

int testMenu();

int prepareBeforeTest();

int allTests();

int withdrawalTest();

int depositTest();

int runTest(int test_type);

#endif //BANK_TEST_H
