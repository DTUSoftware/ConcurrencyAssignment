#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "test.h"
#include "main.h"

int testMain(int argc, char *argv[]) {
    int status = 0;
    if (argc < 2 || !strcmp(argv[1], "menu"))
        status = testMenu();
    else if (!strcmp(argv[1], "all"))
        if ((status = allTests()) == 0)
            printf("All tests completed successfully!\n");
        else
            printf("Tests failed!\n");
    else if (!strcmp(argv[1], "withdrawal"))
        if ((status = withdrawalTest()) == 0)
            printf("Withdrawal test completed successfully!\n");
        else
            printf("Withdrawal test failed!\n");
    else if (!strcmp(argv[1], "deposit"))
        if ((status = depositTest()) == 0)
            printf("Deposit test completed successfully!\n");
        else
            printf("Deposit test failed!\n");
    else if (!strcmp(argv[1], "transfer"))
        if ((status = transferTest()) == 0)
            printf("Transfer test completed successfully!\n");
        else
            printf("Transfer test failed!\n");
    else {
        printf("Usage: bank [-test [menu | all | withdrawal | deposit | transfer]]\n");
        return 1;
    }
    return status;
}

int testMenu() {
    clearScreen();
    printf("=====[ Testing Menu ]=====\n");
    printf("Choose Test:\n\t[0] Exit\n\t[1] Run All Tests\n\t[2] Withdrawal Test\n\t[3] Deposit Test\n\t[4] Transfer Test\n");

    printf("> ");
    fflush(stdout);

    int readChar = getchar();  // Read option from input
    while (getchar() != '\n'); // Flush any excess input out

    int status = 0;

    switch (readChar) {
        case '0': {
            status = 0;
            break;
        }
        case '1': {
            if ((status = allTests()) == 0)
                printf("All tests completed successfully!\n");
            else
                printf("Tests failed!\n");
            break;
        }
        case '2': {
            if ((status = withdrawalTest()) == 0)
                printf("Withdrawal test completed successfully!\n");
            else
                printf("Withdrawal test failed!\n");
            break;
        }
        case '3': {
            if ((status = depositTest()) == 0)
                printf("Deposit test completed successfully!\n");
            else
                printf("Deposit test failed!\n");
            break;
        }
        case '4': {
            if ((status = transferTest()) == 0)
                printf("Transfer test completed successfully!\n");
            else
                printf("Transfer test failed!\n");
            break;
        }
        default: {
            status = 1;
            break;
        }
    }
    return status;
}

int prepareBeforeTest() {
    return 0;
}

int allTests() {
    return (withdrawalTest() && depositTest() && transferTest());
}

int withdrawalTest() {
    int status;
    if ((status = prepareBeforeTest()) != 0) {
        return status;
    }
    return status;
}

int depositTest() {
    int status;
    if ((status = prepareBeforeTest()) != 0) {
        return status;
    }
    return status;
}

int transferTest() {
    int status;
    if ((status = prepareBeforeTest()) != 0) {
        return status;
    }
    return status;
}
