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

#include "../inc/semaphore.h"

void semOp (int semid, unsigned short sem_num, short sem_op) {
    struct sembuf sop = {.sem_num = sem_num, .sem_op = sem_op, .sem_flg = 0};
    semop(semid, &sop, 1);
    //if (semop(semid, &sop, 1) == -1)
        //perror("");
}

void quitSem(int semid) {
    if (semctl(semid, 0, IPC_RMID, NULL) == -1)
        perror("semctl IPC_RMID failed");
}

int createSem(int numSem) {
    return semget(IPC_PRIVATE, numSem, S_IRUSR | S_IWUSR);
}

void initSem(int semid, int n, unsigned short v[]) {
    semid = createSem(n);
    union semun arg;
    arg.array = v;

    semctl(semid, 0, SETALL, arg);
}