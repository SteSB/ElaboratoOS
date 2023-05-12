#ifndef _FIFO_HH
#define _FIFO_HH

void sendIntFIFO(char path[], const int *buf, char name[], size_t size);
void receiveIntFIFO(char path[], int *buf, char name[], size_t size);

void sendStringFIFO(char path[], const char *buf, char name[], size_t size);
void receiveStringFIFO(char path[], char *buf, char name[], size_t size);

#endif