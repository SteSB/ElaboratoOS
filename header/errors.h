#ifndef ERRORS_H
#define ERRORS_H

#include "error.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void errGeneric();
void errCustom(const char *msg);
void endGameError(const char *msg);

#endif 