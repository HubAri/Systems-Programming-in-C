#include "header.h"
#include "init.h"
#include "prolog.h"
#include "config.h"
#include "thinker.h"


  
  
  pid_t pid =0;

  

  //Variables for SHM Segments
  int shmID_serverInfo;
  int shmID_player;
  int shmID_board = -1;
  int* shm_board_address;
  int *shmIDplayer;  //hier einmal als Pointer definiert damit die Variable zum attachen benutzt werden kann

  struct serverinfo *serverinfo;
  
  //variable for pipe
  int pfds[2];


//Hilfsfunktion zum Löschen der SHM Segmente beim Terminieren
static void handleExit(void){
  if(shmIDplayer != 0){
    for(int i=0; i<serverinfo->totalPlayers-1; i++){
      if(shmIDplayer[i]!=0){
        shmIDplayer[i] = deletingSHM(shmIDplayer[i]);
      }
    }
  }
  shmID_player = deletingSHM(shmID_player);
  shmID_serverInfo = deletingSHM(shmID_serverInfo);
  //delete SHM-Segment for board at the end too
  shmID_board = deletingSHM(shmID_board);
  
}

void attachPlayers(int sig){
  (void)sig;
  for (int i= 0; i< serverinfo->totalPlayers-1; i++){
    serverinfo->restPlayers[i] =attachingSHM(shmIDplayer[i]);
  }
}


/*
How To Use: call ./sysprak-client with two parameters: -g <GAME_ID> 
Additionally you may add:  -p <PLAYER_NUMBER> or -c <config_file>
*/
int main(int argc, char **argv)
{
  //create SHM Segments (they must be created before fork() is called)
  shmID_player = creatingSHM(BUFFERLENGTH*sizeof(int)); //vllt auch stattdessen sizeof(struct player)
  shmID_serverInfo = creatingSHM(sizeof(struct serverinfo)); 

  

  //signal for players
  signal(SIGUSR2, attachPlayers); 

  //check if argument count is correct
  if(!check_argc(argc)) return 1;

  //read command line parameters 
  if(!get_args(argc, argv)) return 1;
 
    
  save_config_data();

  
  /*
  Create pipe with paratmeter pfds, short for pipe file descriptors. 
  pfds[0] is the read end and pfds[1] is the write end.
  */
  pipe(pfds);
  


 if((pid=fork())<0){
   perror("Error splitting the process");
   exit(1);

  //CONNECTOR (child process)
  }else if(pid == 0){
    

    //Aufruf Hilfsfunktion zum Löschen der Segmente
    atexit(handleExit);
    //close writing end of pipe
    close(pfds[1]);
    //connect to the MNM Server
    int socket_fd;
    if ((socket_fd = connectServer()) == -1){
      perror("connection");
      return -1;
    }

    //Attachen der SHM Bereiche
    serverinfo = attachingSHM(shmID_serverInfo);
    shmIDplayer = attachingSHM(shmID_player);

    //prolog phase
    if(!prolog(socket_fd)) return 1;

    //game phase 
    if(!game(socket_fd)) return 1;
    

    close(socket_fd);

  
  }else {
    //THINKER(parent process)
    //pid, which is fork()'s return value, is now the child's process-id.
    
    //close reading end of pipe
    close(pfds[0]);
    //SHM Segmente im Thinker attachen
    serverinfo = attachingSHM(shmID_serverInfo);
    shmIDplayer = attachingSHM(shmID_player);
    //Speichern der PID im Struct
    serverinfo->thinker =getpid();
    serverinfo->connector= pid;
    signal(SIGUSR1, thinker);
    //kill the zombie process 
    if(waitpid(pid,  NULL, 0) != pid){
      perror("Error while waiting for childprocess");
      exit(EXIT_FAILURE);
    }
    
    
  }

 return EXIT_SUCCESS;
}

void thinker() {
  if ( serverinfo->calcFlag == 1){
  //if not connected to the board shared memory, then connect now
  if(shmID_board == -1) {
    shmID_board = serverinfo->shm_identifier;
    shm_board_address = attachingSHM(shmID_board);
  }
  
  height = shm_board_address[0];
  width = shm_board_address[1];
  nextPiece = shm_board_address[2];

  //remove nextPiece from list of available pieces to choose from for the opponent
  freePieces[nextPiece] = -1;
  
  //update board by copying it from the 
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      board[i][j] = shm_board_address[4*i + j + 3];
    } 
  }
  calculateMove();
  print_board_binary(height,width, board);
  freePieces[nextPiece] = -1;
  write(pfds[1], nextMove, 16);
  }
  //reset flag to initial state
  serverinfo->calcFlag = 0;
  
}
  
  
 




