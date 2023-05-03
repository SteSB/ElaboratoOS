#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "./header/errors.h"
#include "./header/function.h"

// TODO GLOBALI

// TODO DICHIARAZIONI

void reciveGridDimensions();
void sendInitInfoToServer(char *argv[]);

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        endGameError("\nNome utente non trovato oppure troppi argomenti: ./F$Client nomeUtente\n");
    }
    else
    {

        reciveGridDimensions();
        sendInitInfoToServer(argv);

        while (1)
        {
        }
    }

    return 0;
}

// TODO FUNZIONI

void sendInitInfoToServer(char *argv[])
{
    char *fifoServerToClient = "/tmp/test";
    mkfifo(fifoServerToClient, S_IRUSR | S_IWUSR);
    int fd = open(fifoServerToClient, O_WRONLY);
    write(fd, argv[1], sizeof(argv[1]));
    close(fd);
}

void reciveGridDimensions()
{
    char *fifoServerToClient = "/tmp/fifoServerToClient";
    int res = mkfifo(fifoServerToClient, S_IRUSR | S_IWUSR);

    int fd = open(fifoServerToClient, O_RDONLY);
    char gridDim[2];
    read(fd, gridDim, sizeof(gridDim));
    printf("%s\n", gridDim);
    close(fd);
}