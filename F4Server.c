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

#include "./header/errors.h"
#include "./header/semaphore.h"

// TODO GLOBALI
short timeSigintRecived = 0;

// TODO DICHIARAZIONI
void handlerSigintRecived(int sig);
int checkStartParameters(int argc, char *argv[]);
void reciveClientConferm();
void sendGridDimensions(char *argv[]);
void waitingPlayers(char *argv[]);

int main(int argc, char *argv[])
{
    /*
        Controllo corretta esecuzione del comando (./F4Server 6 7 O X)
             necessari 5 parametri (eseguibile int int char char)
    */
    if (!checkStartParameters(argc, argv))
        endGameError("\nPer avviare il server è necessario eseguire il comando secondo il formato: ./F4Server m n x y. \nDove m e n sono interi, x e y sono i simboli dei giocatori (x != y).\n");

    // * GIOCO
    /*
        Il processo padre dovrà gestire l'eventuale uscita dal gioco (2 volte CTRL + C):
            - creo un set di segnali
            - inizializzo tutti i segnali e rimuovo SIGINT
            - aggiungo il set alla lista dei segnali da bloccare
            - attendo l'arrivo del segnale SIGINT
            - quando lo ricevo invoco handlerSigintRecived
    */
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    sigprocmask(SIG_SETMASK, &set, NULL);

    if (signal(SIGINT, handlerSigintRecived) == SIG_ERR)
        perror("\nSignal error: SIGINT\n");

    waitingPlayers(argv);

    return 0;
}

// TODO FUNZIONI

/*
    Se è stato ricevuto una volta:
        - incremento contatore a 1
        - fornisco indicazioni su come terminare il gioco
    se è stato ricevuto due volte:
        - termino il processo

    !! AGGIUNGERE CONTROLLO SU DEALLOCAZIONE SEMAFORI, SIGNAL...
*/
void handlerSigintRecived(int sig)
{
    if (timeSigintRecived == 0)
    {
        if (sig == SIGINT)
            printf("\nPremere nuovamente CTRL + C per terminare il gioco\n");
        timeSigintRecived++;
    }
    else
    {
        printf("\nGioco terminato\n");
        timeSigintRecived = 0;
        exit(1);
    }
}

/*
    - 5 argomenti
    - le dimensioni non devono essere caratteri
    - le dimensioni non devono essere minori di 5
    - le pedine non devono essere uguali
*/
int checkStartParameters(int argc, char *argv[])
{

    if ((argc != 5 ||
         !(isdigit(*argv[1]) && isdigit(*argv[2]))) ||
        (*argv[1] < '5' && *argv[2] < '5') ||
        (*argv[3] == *argv[4]))
        return 0;
    return 1;
}

void reciveClientConferm()
{
    char *fifoServerToClient = "/tmp/test";
    int res = mkfifo(fifoServerToClient, S_IRUSR | S_IWUSR);
    int fd = open(fifoServerToClient, O_RDONLY);
    char gridDim[50];
    read(fd, gridDim, sizeof(gridDim));
    printf("%s\n", gridDim);
    close(fd);
}

void sendGridDimensions(char *argv[])
{
    char *fifoServerToClient = "/tmp/fifoServerToClient";
    mkfifo(fifoServerToClient, S_IRUSR | S_IWUSR);
    int fd = open(fifoServerToClient, O_WRONLY);
    char gridDim[2] = {*argv[3], *argv[4]};
    write(fd, gridDim, sizeof(gridDim));
    close(fd);
}

void waitingPlayers(char *argv[])
{
    /*
        Genero un id del semaforo (3 elementi [invioDimensioni, ricezioneGiocatore, numeroGiocatori])
    */
    int semid = semget(IPC_PRIVATE, 3, S_IRUSR | S_IWUSR);
    if (semid == -1)
        perror("semget failed");
    /*
        Inizializzo il semaforo: 1, 0, 2.
        Prima invia le dimensioni e attende due giocatori
    */
    unsigned short semInitVal[] = {1, 0, 2};
    union semun arg;
    arg.array = semInitVal;
    if (semctl(semid, 0, SETALL, arg) == -1)
        perror("semctl SETALL failed");

    /*
        Processo figlio:
        serve per l'invio delle dimensioni ai client.
        Sincronizzato con i semafori continua a richiamare sendGridDimensions per inviare
        tramite FIFO al client.
    */
    pid_t sender = fork();
    if (sender == -1)
        printf("Errore");
    else if (sender == 0)
    {
        sigset_t set;
        sigfillset(&set);
        sigprocmask(SIG_SETMASK, &set, NULL);
        while (1)
        {
            /*
                Se semaforo[invioDimensioni] > 0 posso procedere. Altrimenti aspetto.
                sendGridDimensions blocca il processo finchè non vengono inviati i dati.
                Passo il turno alla ricezione del messaggio ack da parte del client
            */
            semOp(semid, 0, -1);
            sendGridDimensions(argv);
            semOp(semid, 1, 1);
        }
        exit(0);
    }

    /*
        Processo figlio:
        serve per la ricezione dei messaggi iniziali da parte del client (nome e altre info per iniziare il gioco).
        Sincronizzato con i semafori continua a richiamare reciveClientConferm per attendere la ricezione.
    */
    pid_t reciver = fork();
    if (reciver == -1)
        printf("Errore");
    else if (reciver == 0)
    {
        sigset_t set;
        sigfillset(&set);
        sigprocmask(SIG_SETMASK, &set, NULL);
        while (1)
        {
            /*
                se semaforo[ricezioneGiocatore] > 0 posso procedere alla ricezione. Altrimenti aspetto.
                Decremento un giocatore (2-1, ... , 1-1) dal semaforo e (in caso semaforo[numeroGiocatori != 0]) passo il turno all'invio 
                delle dimensioni al secondo giocatore.
            */
            semOp(semid, 1, -1);
            reciveClientConferm(argv);
            semOp(semid, 2, -1);
            semOp(semid, 0, 1);
        }
        exit(0);
    }

    /*
        Processo figlio:
        serve per terminare la procedura dello scambio di informazioni iniziale.
        Se semaforo[numeroGiocatori] == 0 allora ci sono tutti i giocatori e posso iniziare il gioco.
    */
    pid_t test = fork();
    if (test == -1)
        printf("Errore");
    else if (test == 0)
    {
        sigset_t set;
        sigfillset(&set);
        sigprocmask(SIG_SETMASK, &set, NULL);
        /*
            Attende che semaforo[numeroGiocatori] sia a 0 e uccide i processi figlio sender e reciver.
        */
        semOp(semid, 2, 0);
        kill(sender, SIGKILL);
        kill(reciver, SIGKILL);

        exit(0);
    }

    /*
        Attendo la fine dei processi figlio.
    */
    int status;
    while (wait(&status) != -1)
    {
    };

    /*
        Dealloco i semafori (IPC_RMID)
    */
    printf("Ci sono tutti i giocatori");
    if (semctl(semid, 0 /*ignored*/, IPC_RMID, NULL) == -1)
        perror("semctl IPC_RMID failed");
}