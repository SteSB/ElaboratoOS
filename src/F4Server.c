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

#include "../inc/semaphore.h"
#include "../inc/errors.h"
#include "../inc/signal.h"
#include "../inc/fifo.h"

typedef struct
{
    int x;
    int y;
} gridDimensions;

typedef struct
{
    char pawn;
    char name[20];
} player;

// TODO: VARIABILI GLOBALI
gridDimensions gridDim;
player players[2]={{.name=""}, {.name=""}};
int timeSigintRecived = 0;
int test = 0;
int test2 = 0;
// char player1[50], player2[50];

// TODO: DICHIARAZIONI
int checkStart(int argc, char *argv[]);
void handlerSigintRecived(int sig);
void quitGame();
void sendStarterInfo();
void receiveStarterInfo();
void inizializeGame();

int main(int argc, char *argv[])
{

    // Controllo correttezza input
    if (!checkStart(argc, argv))
        errCustom("\nPer avviare il server è necessario eseguire il comando secondo il formato: ./F4Server m n x y. \nDove m e n sono interi, x e y sono i simboli dei giocatori (x != y).\n");

    // Inizializzo dimensioni e pedine
    gridDim.x = atoi(argv[1]);
    gridDim.y = atoi(argv[2]);
    players[0].pawn = (char)*argv[3];
    players[1].pawn = (char)*argv[4];

    // Set di segnali senza CTRL + C
    createSetSignal(SIGINT);
    if (signal(SIGINT, handlerSigintRecived) == SIG_ERR)
        errCustom("\nSignal error: SIGINT\n\n");
    
    // GIOCO
    //sendStarterInfo();
    inizializeGame();

    printf("Giocatore 1: %s [%c]\n", players[0].name, players[0].pawn);
    printf("Giocatore 2: %s [%c]\n", players[1].name, players[1].pawn);

    return 0;
}

// TODO: FUNZIONI
int checkStart(int argc, char *argv[])
{
    if ((argc != 5 ||
         !(isdigit(*argv[1]) && isdigit(*argv[2]))) ||
        (*argv[1] < '5' && *argv[2] < '5') ||
        (*argv[3] == *argv[4]))
        return 0;
    return 1;
}

void handlerSigintRecived(int sig)
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
        quitGame();
    }
}

void quitGame()
{
    printf("\n\nGioco terminato\n\n");
    exit(0);
}

void sendStarterInfo() {
    //printf("Invio\n");
    int toSend[] = {gridDim.x, gridDim.y};
    sendIntFIFO("sendGridDim", &toSend[0], "Server", sizeof(toSend));

    test = (test == 0) ? 1 : 2;
    char toSend2[] = {players[test-1].pawn, players[((test+1)%2)-1].pawn};
    sendStringFIFO("sendPawn", &toSend2[0], "Server", sizeof(toSend2));
}

void receiveStarterInfo() {
    test2 = (test2 == 0) ? 1 : 2;

    char toReceive[20];
    receiveStringFIFO("sendName", &toReceive[0], "Server", sizeof(toReceive));
    //printf("Name: %s\n", toReceive);
    strcpy(players[test2-1].name, toReceive);
    
    //printf("<Server> Nome ricevuto: %c & %c\n", players[0].pawn, players[1].pawn);
}

void inizializeGame() {
    /* Gestisce tutte le fasi iniziali:
        - invio dati (griglia e pedine)
        - ricezione nomi
        - inizializzazione di tutto ciò che serve prima che inizi il gioco
    */
    /*unsigned short v[] = {1, 0, 2};
    initSem(semid, 3, v);*/

    sendStarterInfo();
    receiveStarterInfo();
    sendStarterInfo();
    receiveStarterInfo();

}