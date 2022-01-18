#include "header.h"
#include "init.h"
#include "connect.h"
#include "prolog.h"
/*
  char portVal[BUFFERLENGTH_PORT];
  char *paramNameHost = "hostname";
  char *paramNamePort = "portnumber";
  char *paramNameGame = "gamekindname";
  char *string = "";
  char confile [100];
  int test = 0;
 

  configparam confiparam;
*/
 pid_t pid =0;
  //Variables for SHM Segments
  int shmID_serverInfo;
  int shmID_player;
  int *shmIDplayer;  //hier einmal als Pointer definiert damit die Variable zum attachen benutzt werden kann

  struct serverinfo *serverinfo;


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
 
 //save configfile data in struct   
 make_config_file();

  


 if((pid=fork())<0){
   perror("Error splitting the process");
   exit(1);

  //CONNECTOR (child process)
  }else if(pid == 0){
    

    //Aufruf Hilfsfunktion zum Löschen der Segmente
    atexit(handleExit);

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

    //SHM Segmente im Thinker attachen
    serverinfo = attachingSHM(shmID_serverInfo);
    shmIDplayer = attachingSHM(shmID_player);
    //Speichern der PID im Struct
    serverinfo->thinker =getpid();
    serverinfo->connector= pid;

    
    //kill the zombie process 
    if(waitpid(pid,  NULL, 0) != pid){
      perror("Error while waiting for childprocess");
      exit(EXIT_FAILURE);
    }
    
    
  }

 return EXIT_SUCCESS;
}
  
  
 




