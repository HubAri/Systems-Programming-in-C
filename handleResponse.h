#ifndef respHn
#define respHn

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "handleRequest.h"
#include "header.h"
#include "sharedMemoryFunctions.h"

int step;                       //Wächter, dass kein Schritt übersprungen wurde
int end;
extern struct serverinfo *serverinfo;
extern int* shmIDplayer;

char *handle(char *request);

#endif
