#include "../inc/function.h"
#include <string.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void printMatrix( int r, int c, char *m) {
    printf("\n");
    for(int i=0; i<c; i++)
        printf(" %i  ", i);
    printf("\n");
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            printf(" %c |", m[i*c+j] != '\0' ? m[i*c+j] : ' ');
        }
        printf("\n");
    }
    printf("\n");
}