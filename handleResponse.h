#ifndef respHn
#define respHn

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "handleRequest.h"
#include "header.h"
#include "sharedMemoryFunctions.h"

//extern keyword nicht notwendig
//extern char *game_id;
//extern char *player_number;
int step;                       //Wächter, dass kein Schritt übersprungen wurde
extern struct serverinfo *serverinfo;

char *handle(char *request);

#endif
