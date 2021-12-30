#include "handleResponse.h"
#include <unistd.h>

int step = 1;
int countPlayer = 0; 
int sleepCounter = 10;
bool player1 = 0;
bool player2 = 0;

char *handle(char *request){
    char *response;
    char *print;
    


    if((response = (char*) malloc(BUFFERLENGTH*sizeof(char))) == NULL){
        perror("Error allocating memory\n");
    }
    if((print = malloc(BUFFERLENGTH*sizeof(char)))==NULL){
      perror("Error allocating memory\n");
    }
    
    switch(step) {

      //each case expects a different message from the server.
      case 1:

        //check if message is correct
        if(match(request, "MNM Gameserver .+accepting connections")) {

          //extract first digit of the server game version
          sscanf(request, "MNM Gameserver v%s accepting connections", cip_version);
          
          if(cip_version[0] == '2') {
            strcpy(response,"VERSION ");
            strcat(response, OUR_VERSION);

            strcpy(print, "The MNM Gameserver with version ");
            char *version = substring(request, 16,strlen(request)-22);      //Aus dem vom Server gesendeten String die Versionsnummer herausfiltern
            strcat(print, version);
            strcat(print, " accepted the connection. "); 

            if (version != NULL) {
              free(version);
            }

            
            step++;
            
          } else {
            strcpy(response, "Error");
          } 
        } else {
          strcpy(response, "Error");
        }
        break;  
      case 2:
        if(match(request, "Client version accepted - please send Game-ID to join")) {
          strcpy(response, "ID ");
          strcat(response, game_id);

          strcpy(print, "Client version accepted - please send Game-ID to join.");
          
          step++;
          
      
        } else {
          strcpy(response, "Error");
        }
        break;
      case 3:
        if(match(request, "PLAYING .+")) {

          char game[64];
          sscanf(request, "PLAYING %s", game);
          if (strcmp(game, "Quarto")) {
            
            strcpy(response, "Error");
            
          } else {
            free(response);
            response = NULL;

            strcpy(print, "Es wird das Spiel ");
            char *gamekind = substring(request, 8, strlen(request));
            strcat(print, gamekind);
            strcat(print, " gespielt.");

            if (gamekind != NULL)
            {
              free(gamekind);
            }


            step++;
          }
          
          
        } 

        break; 

      case 4:
        if(match(request, ".+")) {
          strcpy(response, "PLAYER ");
          strcat(response, player_number); 

          strcpy(print, "Diese Partie trägt den Namen : \'");
          strcpy(serverinfo->nameOfGame, request); //speichere Spielenamen im Struct
          //char *gamename = substring(request, 0, strlen(request));
          strcat(print, request);
          strcat(print, "\'.");

          step++;
      
        } 
        break;
      case 5:
        if(match(request, "YOU .+ .+")) {

          sscanf(request, "YOU %s Player %s", player_number, player_name);
          free(response);
          response = NULL;

          char *playnum = substring(request, 4, 5);
          char *playname = substring(request, 6, strlen(request)); 

          
          int playNum1 = atoi(playnum)+1;
          char *pointer=malloc(sizeof(char)+1);
          sprintf(pointer, "%d", playNum1); 
                          



          
              

          strcpy(print, "Du (");
          strcat(print, playname );
          strcpy(serverinfo->ourPlayerName,playname);      //Spielername
          strcat(print, ") bist Spieler Nummer ");
          serverinfo->assignedPlayerNumber=atoi(pointer); //Spielernummer
          strcat(print, pointer);
          strcat(print, "!");

          if (playname != NULL)
          {
            free(playname);
          }
          if (playnum != NULL)
          {
            free(playnum);
          }
          
         if (pointer != NULL)
         {
           free(pointer);
         }
       


          step++;

        } else {
          strcpy(response, "Error");
        }
        break;

      
      case 6:
        if(match(request, "TOTAL .+")) {
          free(response);
          response = NULL;

          char *totalPlayer = substring(request, 6, strlen(request));
          int anz = atoi(totalPlayer);
          serverinfo->totalPlayers = atoi(totalPlayer); //Totale Anzahl der Spieler ins Struct
          if(anz > 1){
            strcpy(print, "Es nehmen ");
          }else{
            strcpy(print, "Es nimmt ");
          }
          strcat(print, totalPlayer);
          strcat(print, " Spieler am Spiel teil.");
          //Anlegen von SHM Segmenten für jeden Player, da hier Totalplayeranzahl bekannt
          for (int i=0; i<serverinfo->totalPlayers-1; i++){
            shmIDplayer[i] =creatingSHM(sizeof(struct player));
            serverinfo->restPlayers[i] = attachingSHM(shmIDplayer[i]);
          }


          if (totalPlayer != NULL)
          {
            free(totalPlayer);
          }


          step++;
        } else {
          strcpy(response, "Error");
        }
        
        break;
      case 7:
        if(match(request, ".+ .+ .+")) { 
          free(response);
          response = NULL;

          char *playNum = substring(request, 0, 1); 
          char *playName = substring(request, 2, strlen(request)-2);
          char *status = substring(request, strlen(request)-1,strlen(request) );

          int playNum1=atoi(playNum)+1;
          char playNumC=playNum1+'0';
          char *point=&playNumC;
          
          serverinfo->restPlayers[countPlayer]->playerNumber = atoi(playNum);
          serverinfo->restPlayers[countPlayer]->ready = atoi(status);
          strcpy(serverinfo->restPlayers[countPlayer]->playerName, playName);
          
          strcpy(print, "Spieler Nummer ");
          strcat(print, point);
          strcat(print, " (");
          strcat(print, playName);
          strcat(print, ") ist ");
          
          
          if(atoi(status)==1){
            strcat(print, "bereit!");
          }else if(atoi(status)==0){
            strcat(print, "nicht bereit!");
          }

          if (playNum != NULL){
            free(playNum);
          }
          if (playName != NULL){
            free(playName);
          }
          if (status != NULL){
            free(status);
          }

         countPlayer++;
          
        } else if (match(request, "ENDPLAYERS")) {
          free(response);
          response = NULL;
          strcpy(print, "Viel Spaß beim Spielen!");
          step++;
        } else {
          strcpy(response, "Error");
        }
        step++;
        break;

      
      case 8:  //Unterscheidung in welchen case wir springen
        if(match(request, "MOVE")){   
          sleepCounter = 10; 
          step = 9;
        }  
        if(match(request, "WAIT")){   
          step = 10;
        } 
        if(match(request, "GAMEOVER")){   
          step = 11;
        } else {  //?
          strcpy(response, "Error");
        }
        break; 


      case 9:           //Server hat Move geschickt
        //server schickt das Spielfeld, so lange warten
        while(!match(request, "ENDFIELD")){
          //Dauerschleife bis Schlüsselwort kommt
        }
        
        //hier einlesen des Spielfelds depending on Thniking-Method

        //dem Server Antworten
        strcpy(response, "THINKING");

        //Jetzt Aufgabe Phase Spielzug (Spielzug berechnen und senden) - aufruf der Methode inkl. Senden des Spielzugs

        step = 8;
        break;




      case 10:    //Wait-Zweig
        strcpy(response, "OKWAIT");
        sleep(1);
        sleepCounter--;
        if (sleepCounter <= 0){
          strcpy(response, "Error");
        }else{
          step = 8;
        }

        break;


      case 11:   //Gameover 

        if(match(request, "PLAYER0WON")){  
          
          if(response != NULL){
            free(response);
          }

          char *won0 = substring(request, 11, strlen(request));
          if(!strcmp(won0, "Yes")){
            player1 = 1;
          } 
          free(won0);

        } else if(match(request, "PLAYER1WON")) {
          
          if(response != NULL){
            free(response);
          }

          char *won1 = substring(request, 11, strlen(request));
          if(!strcmp(won1, "Yes")){
            player2 = 1;
          } 
          free(won1);
        } 

        if(match(request, "QUIT")){   
          
          if(player1 && !player2){ //erster spieler
 
           strcpy(print, "Player 1 won the game.");

          } else if(!player1 && player2){ //zweiter spieler

            strcpy(print, "Player 2 won the game.");

          } else { //untentschieden

            strcpy(print, "The game is undecided."); 
          }

        } 

        break;

      default:
        free(response);
        response = NULL;
        break;
    }

    if(response != NULL) {
      strcat(response, "\n");
    }

    if(print != NULL){
      printf("S: %s\n", print);
      free(print);
    }

    
    return  response;
    
}


