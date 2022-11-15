#ifndef BANK_TEST_H
#define BANK_TEST_H

#define MAX_THREAD_AMOUNT 10
#define RANDOM_TESTS_PER_RUN 50

int testMain(int argc, char *argv[]);

int testMenu();

int prepareBeforeTest();

int allTests();

int withdrawalTest();

int depositTest();

int transferTest();

#endif //BANK_TEST_H
