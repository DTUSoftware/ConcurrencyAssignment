#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "main.h"
#include <limits.h>
#include <math.h>


int convertStrToInt(char *string, int *integer) {
    // thanks stackoverflow https://stackoverflow.com/a/3068420
    if (strlen(string) > (floor(log10(abs(INT_MAX))) + 1)) {
        printf("You exceeded the maximum value of an integer!\n");
        return ERROR;
    }

    // TODO: check for integers over - strtol

    // Convert buffered string to integer
    *integer = atoi(string);
    return OK;
}

int getCustomValue(int *value) {
    printf("Write desired amount:\n");

    printf("> ");
    fflush(stdout);

    char **bufferptr = malloc(sizeof(char *));
    assert(bufferptr != NULL);

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    assert(buffer != NULL);
    bufferptr[0] = buffer;

    // Read input from console
    if (readinput(bufferptr, 1, stdin) != OK) {
        // if we could not read, and not caused by memory error, try again
        free(buffer);
        return 1;
    }
    buffer = *bufferptr;
    free(bufferptr);

    if (convertStrToInt(buffer, value) != OK) {
        free(buffer);
        return ERROR;
    }
    free(buffer);
    return OK;
}

int clearScreen() {
    // https://www.geeksforgeeks.org/clear-console-c-language/
    printf("\e[1;1H\e[2J");
    return OK;
}

// Reads input from STDIN, while keeping a buffer, in order to
// read "unlimited" input
// IMPORTANT: From our Shell Assignment
int readinput(char **bufferptr, int newlinestop, FILE *stream) {
    char *buffer = *bufferptr;
    // We tried using getline(), but it wasn't in MingW, and stumbled upon this implementation that
    // could not really be altered - so credit where credit is due.
    // thanks to https://gist.github.com/btmills/4201660
    unsigned int buffer_size = BUFFER_SIZE;
    int ch = EOF;
    int pos = 0;

    while (!((ch = fgetc(stream)) == '\n' && newlinestop == 1) && ch != EOF && !feof(stream)) {
        buffer[pos++] = ch;
        if (pos == buffer_size) {
            buffer_size = buffer_size + BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size * sizeof(char));
            assert(buffer != NULL);
        }
    }
    buffer_size = ++pos;
    buffer = realloc(buffer, buffer_size + 1 * sizeof(char));
    assert(buffer != NULL);
    buffer[pos] = '\0';

//            printf("DEBUG: %s\n", buffer);
    bufferptr[0] = buffer;
    return OK;
}

int randNum(int lower, int upper, int *randNum) {
    *randNum = (rand() % (upper - lower + 1)) + lower;
    return OK;
}
