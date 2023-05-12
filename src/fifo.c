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

#include "../inc/fifo.h"

void sendIntFIFO(char path[], const int *buf, char name[], size_t size) {
    //printf("\n");


    //printf("<%s> Creazione FIFO %s...\n", name, path);
    char *pathFIFO = path;
    int fifo = mkfifo(pathFIFO, S_IRUSR | S_IWUSR | S_IWGRP);

    //printf("<%s> Apertura FIFO in scrittura...\n", name);
    int fdFIFO = open(pathFIFO, O_WRONLY);

    //printf("<%s> Invio dati...\n", name);
    write(fdFIFO, &buf[0], size);

    //printf("<%s> Chiusura FIFO...\n", name);
    close(fdFIFO);

    if(fifo != -1) {
        //printf("<%s> Deallocazione FIFO...\n", name);
        unlink(pathFIFO);
    }

    //printf("\n");
    /*printf("%s\n", fifo);
    printf("%i", *buf);
    printf("%i", *(buf+1));
    printf("%li", sizeof(buf));*/
}

void receiveIntFIFO(char path[], int *buf, char name[], size_t size) {
    //printf("\n");

    // printf("<%s> Creazione FIFO %s...\n", name, path);
    char *pathFIFO = path;
    int fifo = mkfifo(pathFIFO, S_IRUSR | S_IWUSR | S_IWGRP);

    if(fifo != -1)
        printf("<%s> In attesa del server...\n", name);

    //printf("<%s> Apertura FIFO in lettura...\n", name);
    int fdFIFO = open(pathFIFO, O_RDONLY);
    
    //printf("<%s> In attesa dei dati...\n", name);
    read(fdFIFO, &buf[0], size);

    //printf("<%s> Chiusura FIFO...\n", name);
    close(fdFIFO);

    if(fifo != -1) {
        //printf("<%s> Deallocazione FIFO...\n", name);
        unlink(pathFIFO);
    }

    //printf("\n");
}


void sendStringFIFO(char path[], const char *buf, char name[], size_t size) {
    //printf("\n");

    //printf("<%s> Creazione FIFO %s...\n", name, path);
    char *pathFIFO = path;
    int fifo = mkfifo(pathFIFO, S_IRUSR | S_IWUSR | S_IWGRP);

    //printf("<%s> Apertura FIFO in scrittura...\n", name);
    int fdFIFO = open(pathFIFO, O_WRONLY);

    //printf("<%s> Invio dati...\n", name);
    write(fdFIFO, &buf[0], size);

    //printf("<%s> Chiusura FIFO...\n", name);
    close(fdFIFO);

    if(fifo != -1) {
        //printf("<%s> Deallocazione FIFO...\n", name);
        unlink(pathFIFO);
    }

    //printf("\n");
}


void receiveStringFIFO(char path[], char *buf, char name[], size_t size) {
    //printf("\n");

    // printf("<%s> Creazione FIFO %s...\n", name, path);
    char *pathFIFO = path;
    int fifo = mkfifo(pathFIFO, S_IRUSR | S_IWUSR | S_IWGRP);

    
    if(fifo != -1)
        printf("<%s> In attesa del server...\n", name);

    //printf("<%s> Apertura FIFO in lettura...\n", name);
    int fdFIFO = open(pathFIFO, O_RDONLY);

    //printf("<%s> In attesa dei dati...\n", name);
    read(fdFIFO, &buf[0], size);

    //printf("<%s> Chiusura FIFO...\n", name);
    close(fdFIFO);

    if(fifo != -1) {
        //printf("<%s> Deallocazione FIFO...\n", name);
        unlink(pathFIFO);
    }

    //printf("\n");
}