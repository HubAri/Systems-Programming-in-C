#include "header.h"
#include "init.h"

//Variables for command line arguments
char game_id[ID_LEN + 1];
char player_number[2];
char player_name[NAME_LEN + 1];
char cip_version[VERSION_LEN + 1];
int player_check = 0; 

//variables for config file
int test=0;
char portVal[BUFFERLENGTH_PORT];
char *paramNameHost = "hostname";
char *paramNamePort = "portnumber";
char *paramNameGame = "gamekindname";
char *string = "";
char confile [100];
configparam confiparam;





bool check_argc(int argc) {
  if(argc != 3 && argc != 5) {
    perror("argc should be 3 or 5");
    return false;
  }
  return true;
}

bool get_args(int argc, char **argv) {
  
  int ret;

  //read parameters 
  while ((ret = getopt(argc, argv, "g:p:c:")) != -1) {
    switch(ret) {
      case 'g':
        if(is_valid_id(optarg)) {
          strcpy(game_id, optarg);
          break;
        } else {
          perror("game id incorrect");
          return false;
        }

      case 'p':
        if(is_valid_player_number(optarg)){
                   
          switch (atoi(optarg)) {
            case 1:
              strcpy(player_number, "0");
              break;
            case 2:
              strcpy(player_number, "1");
              break;  
            
            default:
              break;
            }
            break;
              
          } else {
            perror("player number incorrect");          
            return false;
          }
        
     case 'c':
         if(is_valid_file(optarg, string)){
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

  return true;

} 

void make_config_file() {
  //fill struct
   char *hostValue = readConfig(paramNameHost, confile);
   memset(confiparam.hostName, '\0', sizeof(confiparam.hostName));
   strcpy(confiparam.hostName,hostValue);
   free(hostValue);

  char *gameKindValue = readConfig(paramNameGame, confile);
  memset(confiparam.gameKindName, '\0', sizeof(confiparam.gameKindName));
  strcpy(confiparam.gameKindName,gameKindValue);
  free(gameKindValue);

  char *portValue = readConfig(paramNamePort, confile);
  memset(portVal, '\0', sizeof(portVal));
  strcpy(portVal, portValue);
  free(portValue);
  confiparam.portNumber = atoi(portVal);
  
}

char* readConfig(char* name, char *config) {

    FILE *file = NULL;       
    char *stringV = NULL;
    const char *delim = "= \n";                                                   
    char string[BUFFERLENGTH];  
    char *res = calloc(BUFFERLENGTH, sizeof(char)+1);
    char *pointer;                                        

    file = fopen(config, "r");                                //open config

    if(file != NULL) {                                        //filter error: couldn't open file
        
         while(fgets(string,BUFFERLENGTH,file)) {               //reads a line from the specified stream and stores it into the string pointed to by string
            if((strstr(string, name)) != NULL) {               // filter // finds the first occurrence of string in name 

                pointer = strtok(string,delim);

                while(pointer != NULL) {
                  stringV = pointer;
                  pointer = strtok(NULL, delim);
                }
                
                break;

            } 
        }
    } else {                                                                        
        perror("Error: Couldn't open file!");
        exit(EXIT_FAILURE);
        
   }

  fclose (file);             //close config                                                  

  memset(res, '\0', sizeof(res)+1);
  strcpy(res, stringV);
  return res;
  
}

void createClientConfig(char *confile) {

  FILE *file = NULL;

  file = fopen(confile, "r");                                                                               //open file

  if(file != NULL) {

    fclose(file);                                                                                           //close file

  } else {

    printf("Creating client.conf \n");           
    file = fopen(confile, "w");                                                                             //create client.conf

    if(file == NULL) {                                                                                      //check whether the file can be accessed

  	   perror("Error");  

    } else {

      fprintf(file, "hostname=%s\nportnumber=%i\ngamekindname=%s", HOSTNAME,PORTNUMBER,GAMEKINDNAME);
      fclose(file);                                                                                        //close file

    }
  }
}

  