#include "header.h"
#include "serverConnectionHeader.h"
#include "config.h" 

//declare important variables
  char game_id[ID_LEN + 1];
  char player_number[2];
  char player_name[NAME_LEN + 1];
  char cip_version[VERSION_LEN + 1];
  char portVal[BUFFERLENGTH_PORT];
  char *paramNameHost = "hostname";
  char *paramNamePort = "portnumber";
  char *paramNameGame = "gamekindname";
  char *string = "";
  char confile [100];
  int test = 0;
  pid_t pid =0;

  configparam confiparam;

/*How-To-Use: call ./sysprak-client with two obligatory parameters: 
-g <GAME_ID>  and two optional parameters: -p <PLAYER_NUMBER>
*/
int main(int argc, char **argv)
{

  //two parameters are optional, so there are 3 or 5 parameters in total.
  if(argc != 3 && argc != 5) {
    fprintf(stderr, "Incorrect number of arguments!\n");
    return EXIT_FAILURE;
  }
  
  
  
  // schaut ob spieler als parameter mitgegeben wurde, bei 0 = kein Spieler und bei 1 = Spieler
  int player_check = 0; 
  int ret;

  //get parameters
  while ((ret = getopt(argc, argv, "g:p:c:")) != -1) {
    switch(ret) {
      case 'g':
        if(is_valid_id(optarg)) {
          strcpy(game_id, optarg);
          break;
        } else {
          fprintf(stderr, "The game ID entered is incorrect!\n");
          return EXIT_FAILURE;
        }

      case 'p':
        if(is_valid_player_number(optarg)){
                   
          switch (atoi(optarg))
          {
          case 1:
            strcpy(player_number, "0");
            break;
          case 2:
            strcpy(player_number, "1");
            break;  
          
          default:
            break;
          }
            
          //strcpy(player_number,optarg);
          player_check = 1;
          break;
        } else {
          fprintf(stderr, "The player number you entered is incorrect!");          
          return EXIT_FAILURE;
        }
       
      case 'c':
         if(is_valid_file(optarg,string)){
        // if(strcmp(optarg,string) != 0){
            memset(confile, '\0', sizeof(confile));
            strcpy(confile, optarg);
            printf("The confile is %s. \n", confile);
            test = 1;
        } 
      break;

      default:
        break;

    }
  }

    
 if(test == 0){
   const char *conf = "client.conf";
    memset(confile, '\0', sizeof(confile));
    strcpy(confile, conf);                                                
    createClientConfig(confile);
    printf("Using \"client.conf\" as confile. \n");
 }

  if(player_check == 0){
    strcpy(player_number,"");
    printf("No player set\n");
  }

  //fill struct
  char *hostValue = readConfig(paramNameHost, confile);
  memset(confiparam.hostName, '\0', sizeof(confiparam.hostName));
  strcpy(confiparam.hostName,hostValue);
  hostValue = NULL;

  char *gameKindValue = readConfig(paramNameGame, confile);
  memset(confiparam.gameKindName, '\0', sizeof(confiparam.gameKindName));
  strcpy(confiparam.gameKindName,gameKindValue);
  gameKindValue = NULL;

  char *portValue = readConfig(paramNamePort, confile);
  memset(portVal, '\0', sizeof(portVal));
  strcpy(portVal, portValue);
  portValue = NULL;
  confiparam.portNumber = atoi(portVal);


 if((pid=fork())<0){
   perror("Error splitting the process");
   exit(EXIT_FAILURE);

  }else if(pid == 0){
    //CONNECTOR
    //connect to the MNM Server
    int socket_fd;
    if ((socket_fd = connectServer()) == -1){
      perror("connection");
    }
    //Phase1: the prolog interchange with the server
    performConnection(socket_fd);
    //close(socket_fd);
  }else {
    //THINKER
    
    
  }

 return EXIT_SUCCESS;
}
  
  
 




