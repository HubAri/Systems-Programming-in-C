#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>

#include "header.h"

//Rechte und Key des SHM-Segments
#define SMHKEY IPC_PRIVATE
#define PERMISSIONS 0666

struct serverinfo{
    char nameOfGame[BUFFERLENGTH];
    int  assignedPlayerNumber;
    char totalPlayers;
    pid_t connector;
    pid_t thinker;
};

struct player{
    int playerNumber;
    char playerName[BUFFERLENGTH];
    int ready;
};

//functions
int creatingSHM(size_t size);
void *attachingSHM(int shmID);
void detachingSHM( void *shmAdress);
int deletingSHM (int shmID);