#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/sem.h>

#include "../inc/signal.h"

void quitSignal(int signum) {
    if(signal(signum, SIG_DFL) == SIG_ERR)
        perror("errore signal reset");
}

void createSetSignal(int sigToDel) {
    sigset_t set;
    sigfillset(&set);
    if(sigToDel != 0)
        sigdelset(&set, sigToDel);
    sigprocmask(SIG_SETMASK, &set, NULL);
}

void disabledSignal() {
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_SETMASK, &set, NULL);
}