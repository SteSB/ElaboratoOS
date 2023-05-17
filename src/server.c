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

int timeSigintRecived = 0;
game gameInfo = {.numPlayers = 0};
// char pathClientFIFO[] = {" ", " "};

/*
    PROCESSI FIGLI
    - gestione ricezione e invio al client durante la partita
    - gestione di inizializzazione del gioco
    - controllo sul campo per verificare la vittoria
*/

int checkStartParamsFunction(int argc, char *argv[]);
void getSignalToExitFunction(int sig);
void exitGameFunction();
void postAndGetInizializeInfoFunction();

int main(int argc, char *argv[])
{

    if (!checkStartParamsFunction(argc, argv))
        errCustom("\nPer avviare il server è necessario eseguire il comando secondo il formato: ./F4Server m n x y. \nDove m e n sono interi, x e y sono i simboli dei giocatori (x != y).\n");

    createSetSignal(SIGINT);
    if (signal(SIGINT, getSignalToExitFunction) == SIG_ERR)
        errCustom("\nSignal error: SIGINT\n\n");

    printf("<Server> Il gioco può iniziare...\n");
    gameInfo.gridDim.x = atoi(argv[1]);
    gameInfo.gridDim.y = atoi(argv[2]);
    gameInfo.pawns[0] = (char)*argv[3];
    gameInfo.pawns[1] = (char)*argv[4];
    gameInfo.numPlayers = 0;

    postAndGetInizializeInfoFunction();

    printf("<Server> Inizializzazione effettuata: \n         Griglia [%i x %i]\n         Pedine: %c - %c\n         Giocatori: %s - %s\n", gameInfo.gridDim.x, gameInfo.gridDim.y, gameInfo.pawns[0], gameInfo.pawns[1], gameInfo.players[0].name, gameInfo.players[1].name);

    while (1)
        ;

    return 0;
}

void getSignalToExitFunction(int sig)
{
    if (timeSigintRecived == 0)
    {
        if (sig == SIGINT)
            printf("\n\nPremere nuovamente CTRL + C per terminare il gioco\n\n");
        timeSigintRecived++;
    }
    else
    {
        timeSigintRecived = 0;
        printf("Chiusura gioco\n");
        exitGameFunction();
    }
}

int checkStartParamsFunction(int argc, char *argv[])
{
    if ((argc != 5 ||
         !(isdigit(*argv[1]) && isdigit(*argv[2]))) ||
        (*argv[1] < '5' && *argv[2] < '5') ||
        (*argv[3] == *argv[4]))
        return 0;
    return 1;
}

void exitGameFunction()
{
    exit(0);
}

void postAndGetInizializeInfoFunction()
{
    char inizializePathFIFO[] = "inizializePathFIFO";

    // invio prima le pedine, ottengo i FIFO per comunicare con i singoli client
    // ? PEDINE
    char generalPathFIFO1[] = "scFIFO";
    char toPost1[2] = {gameInfo.pawns[gameInfo.numPlayers], gameInfo.pawns[!(gameInfo.numPlayers)]};
    sendStringFIFO(inizializePathFIFO, &toPost1[0], "Server", sizeof(toPost1));
    receiveIntFIFO(inizializePathFIFO, NULL, "Server", sizeof(int)); // ACK
    strncat(generalPathFIFO1, &gameInfo.pawns[gameInfo.numPlayers], 1);
    strcpy(gameInfo.players[gameInfo.numPlayers].pathFIFO, generalPathFIFO1);
    gameInfo.numPlayers++;

    char generalPathFIFO2[] = "scFIFO";
    char toPost2[2] = {gameInfo.pawns[gameInfo.numPlayers], gameInfo.pawns[!(gameInfo.numPlayers)]};
    sendStringFIFO(inizializePathFIFO, &toPost2[0], "Server", sizeof(toPost2));
    receiveIntFIFO(inizializePathFIFO, NULL, "Server", sizeof(int)); // ACK
    strncat(generalPathFIFO2, &gameInfo.pawns[gameInfo.numPlayers], 1);
    strcpy(gameInfo.players[gameInfo.numPlayers].pathFIFO, generalPathFIFO2);
    gameInfo.numPlayers++;

    // INVIO LE ALTRE INFORMAZINOI
    for (int i = 0; i < 2; i++)
    {
        // ? DIMENSIONI
        int dim[] = {gameInfo.gridDim.x, gameInfo.gridDim.y};
        sendIntFIFO(gameInfo.players[i].pathFIFO, &dim[0], "Server", sizeof(dim));

        // ? NOMI
        receiveStringFIFO(gameInfo.players[i].pathFIFO, &(gameInfo.players[i].name)[0], "Server", sizeof(char) * 50);
    }

    // PRIMO TURNO
    srand(time(NULL));
    gameInfo.turn = rand() % 2;
}
