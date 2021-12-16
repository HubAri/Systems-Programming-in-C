#include "sharedMemoryFunctions.h"


int creatingSHM(size_t size){
 int shmID;
 shmID = shmget(SMHKEY, size, PERMISSIONS);
 if (shmID == -1){
     perror("Shared memory segment could not be created!");
     exit(EXIT_FAILURE);
 }
 return shmID;

}

void *attachingSHM( int shmID){
    char *shmAdress;
    shmAdress = shmat(shmID, 0, 0);
    if (shmAdress == (char *) (-1)){
        perror("Error trying to attach shared memory segment!");
        exit(EXIT_FAILURE);
    }
 return shmAdress;
}

int deletingSHM (int shmID){
    int res;
    res = shmctl(shmID, IPC_RMID, NULL);
    if (res != 0){
        perror("Could not delete Shared Memory segment!");
        exit (EXIT_FAILURE);
    }
    return res;
}

void detachingSHM (void *shmAdress){
    if ((shmdt(shmAdress)) == -1){
        perror("Could not detach from shared memory segment!");
        exit (EXIT_FAILURE);
    }
}