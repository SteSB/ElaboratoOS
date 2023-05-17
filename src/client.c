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
#include <sys/shm.h>

#include "../inc/semaphore.h"
#include "../inc/errors.h"
#include "../inc/signal.h"
#include "../inc/fifo.h"
#include "../inc/function.h"

typedef struct
{
    int x;
    int y;
} grid;

typedef struct
{
    char name[50];
    char pathFIFO[50];
} player;

typedef struct
{
    grid gridDim;
    player players[2];
    char pawns[2];
    int turn;
    int numPlayers;
} game;

game gameInfo;
char *gridGame;
// char gameGrid[][];

void postAndGetInizializeInfoFunction();

int main(int argc, char *argv[])
{

    strcpy(gameInfo.players[0].name, argv[1]);

    /*int toGet[] = {0, 0};
    receiveIntFIFO(inizializePathFIFO, &toGet[0], "Client", sizeof(toGet));
    gameInfo.gridDim.x = toGet[0];
    gameInfo.gridDim.y = toGet[1];

    sendStringFIFO(inizializePathFIFO, &(gameInfo.players[0].name)[0], "Client", sizeof(argv[1]));

    printf("%i %i\n", gameInfo.gridDim.x,gameInfo.gridDim.y );*/
    postAndGetInizializeInfoFunction();

    gridGame = (char *)malloc(gameInfo.gridDim.x * gameInfo.gridDim.y * sizeof(char));
    printMatrix(gameInfo.gridDim.x, gameInfo.gridDim.y, gridGame);

    return 0;
}

void postAndGetInizializeInfoFunction()
{
    char inizializePathFIFO[] = "inizializePathFIFO";

    receiveStringFIFO(inizializePathFIFO, &gameInfo.pawns[0], "Client", sizeof(char) * 2);
    sendIntFIFO(inizializePathFIFO, NULL, "Client", sizeof(int));

    for (int i = 0; i < 2; i++)
    {
        char generalPathFIFO[] = "scFIFO";
        strncat(generalPathFIFO, &gameInfo.pawns[i], 1);
        strcpy(gameInfo.players[i].pathFIFO, generalPathFIFO);
    }

    // RICEVO LE ALTRE INFORMAZIONI
    // ? DIMENSIONI
    int toGet[] = {0, 0};
    receiveIntFIFO(gameInfo.players[0].pathFIFO, &toGet[0], "Client", sizeof(toGet));
    gameInfo.gridDim.x = toGet[0];
    gameInfo.gridDim.y = toGet[1];

    // ? NOME
    sendStringFIFO(gameInfo.players[0].pathFIFO, &(gameInfo.players[0].name)[0], "Client", sizeof(gameInfo.players[0].name));
}