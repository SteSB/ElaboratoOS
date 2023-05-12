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

// TODO GLOBALI
gridDimensions gridDim;
char pawn[2];
char player[20] = "";

// TODO DICHIARAZIONI
void riceiveStarterInfo();
void sendStarterInfo();

int main(int argc, char *argv[]) {

    strcpy(player, argv[1]);
    riceiveStarterInfo();
    sendStarterInfo();
    
    printf("%s: %c\n", player, pawn[0]);

    return 0;
}

// TODO FUNZIONI
void riceiveStarterInfo() {
    int toReceive[] = {0, 0};
    receiveIntFIFO("sendGridDim", &toReceive[0], "Client", sizeof(toReceive));
    gridDim.x = toReceive[0];
    gridDim.y = toReceive[1];
    printf("<Client> Dimensioni griglia ricevute: %i x %i\n", gridDim.x, gridDim.y);


    char toReceive2[2] = {'O', 'X'};
    receiveStringFIFO("sendPawn", &toReceive2[0], "Client", sizeof(toReceive2));
    pawn[0] = toReceive2[0];
    pawn[1] = toReceive2[1];
    // printf("<Client> Pedine ricevute: %c & %c\n", players[0].pawn, players[1].pawn);
}

void sendStarterInfo() {
    printf("Invio nome\n");
    
    sendStringFIFO("sendName", &player[0], "Client", sizeof(player));
}

