#include "config.h"


char portVal[BUFFERLENGTH_PORT];
char *paramNameHost = "hostname";
char *paramNamePort = "portnumber";
char *paramNameGame = "gamekindname";
char confile [100];

configparam confiparam;

char* readConfig(char* name, char *config) {

    FILE *file = NULL;       
    char *stringV = NULL;
    const char *delim = "= \n";                                                   
    char string[BUFFERLENGTH];  
    char *res = calloc(BUFFERLENGTH, sizeof(char)+1);                                         

    file = fopen(config, "r");                                //open config

    if(file != NULL) {                                        //filter error: couldn't open file
        
         while(fgets(string,BUFFERLENGTH,file)) {               //reads a line from the specified stream and stores it into the string pointed to by string
            if((strstr(string, name)) != NULL) {               // filter // finds the first occurrence of string in name 

                stringV = findValueParam(delim, string);   //function returns value of the parameter
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


char* findValueParam(const char *delim, char *string) {  

    char *pointer = NULL;                                                                 
    char *res = NULL;

    pointer = strtok(string, delim);            //breaks string into a series of tokens using delim

    while(pointer != NULL) {                             //while pointer is not finished

      res = pointer;
      pointer = strtok(NULL, delim);            //pointer moves one further

    }
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

void save_config_data(){
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
