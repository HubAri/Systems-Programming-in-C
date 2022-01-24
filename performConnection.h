#ifndef perfCon
#define perfCon

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>


#include "handleRequest.h"
#include "header.h"
#include "handleResponse.h"

int end;                                             //Hier wird gespeichert, ob Ende der Prologphase erreicht ist (Server sendet +ENDPLAYERS)
int socketData;                                      //überprüft, ob Daten am Socket sind
struct timeval tv;                                   //Struct zum Speichern des Zeitintervalls, in dem auf neue Daten überprüft werden soll
fd_set readfd;                                       // Menge von Filedeskriptoren, Sinvoll wenn Pipe hinzukommt, daher jetzt schon genutzt
int retval;


void performConnection(int fileSock);               //Zuständig für die Prologphase
void sendResponse(char *response, int fileSock);    //Sendet Antwort an Server 
void processInformation(char *buffer, int fileSock);
bool prolog(int);
bool game(int, int);

#endif