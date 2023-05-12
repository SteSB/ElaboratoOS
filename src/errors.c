#include "../inc/errors.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void errGeneric() {
    perror("Error");
    exit(1);
}

void errCustom(const char *msg) {
    perror(msg);
    exit(1);
}