#include "handleResponse.h"
#include <unistd.h>

int step = 1;
int countPlayer = 0; 
int move = 0;
bool player1 = false;
bool player2 = false;
int breite = 4;
int hoehe = 4;
int sleepCounter = 10;

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
        break;

      
      case 8:  //Unterscheidung in welchen case wir springen
        if(match(request, "MOVE")){   
          step = 9;
          sleepCounter = 10;   
        }  
        if(match(request, "WAIT")){   
          step = 10;
        } 
        if(match(request, "GAMEOVER")){   //oder ist es doch das Wort Quit
          step = 11;
        } else {  //?
          strcpy(response, "Error");
        }
        break; 


      case 9:    //Move-zweig    //fehlt:siehe MSBlatt1 + ist MOVEOK an der richtigen stelle? 
        //Eigentlicher Move-Zweig  
        move = 1;
        int zeitMax = 5; //könnte auch was anderes sein: Maximale Zugzeit; muss noch implementiert werden

        strcpy(print,"MOVE"); 
        sprintf(print,"%i", zeitMax);

        strcpy(print,"NEXT"); 
        strcpy(print, ""); //zu setzender Spielstein //austauschen mit variable/o.a.

        strcpy(print,"FIELD"); //immer 4x4
        sprintf(print,"%i", breite);
        strcat(print, ", ");
        sprintf(print,"%i", hoehe);

        // fehlt: siehe Meilenstein1 Blatt ?? Funktion printFeld() wäre an der Stelle super!

        strcpy(print,"ENDFIELD");

        //thinking

        if(response != NULL){
          free(response);
        }
        response = NULL;

        if(print != NULL){ 
          free(print);
        }
        print = NULL;
        
        //hier muss doch noch der Move gesendet werden

        if(match(request, "MOVEOK")){
          if(response!=NULL){
            free(response);
          }
          response = NULL;
          strcpy(print,"Move is vaild");
        }
        step = 8;
        break;

      case 10:    //Wait-Zweig
        strcpy(response, "OKWAIT");
        strcpy(print,"wait"); //fraglich ob nötig
        sleep(1);
        sleepCounter--;
        if (sleepCounter <= 0){
          strcpy(response, "Error");
        }else{
          step = 8;
        }
        break;


      case 11:   //Gameover 
//sollte vom Server ermittelt werden wer gewonnen hat

        strcpy(print,"GAMEOVER  [[ ");  

        //gewinner ermitteln: ka ob das stimmt
        char *won1 = substring(request, 11, strlen(request)); //11?
        if (!strcmp(won1, "Yes")){  //"Yes"?
          player1 = true;
        } else {
          player1 = false;
        }

        char *won2 = substring(request, 11, strlen(request)); //11?
        if (!strcmp(won2, "Yes")){  //"Yes"?
          player2 = true;
        } else {
          player2 = false;
        }
        //fehlt spielername + nummer des gewinners

          char *playnum = substring(request, 4, 5);
          char *playname = substring(request, 6, strlen(request)); 

          
          int playNum1 = atoi(playnum)+1;
          char *pointer=malloc(sizeof(char)+1);
          sprintf(pointer, "%d", playNum1); 


        if(player1 && !player2){ //erster spieler
 
          strcpy(print, playname); 
          strcpy(print, pointer);

        } else if(!player1 && player2){ //zweiter spieler
  
          strcpy(print,playname); //strcat ?
          strcpy(print, pointer);

        } else { //untentschieden

          strcpy(print, "undecided game"); //es wird kein Gewinner angegeben

        }

        strcat(print, " ]]");

        strcpy(print,"FIELD"); //immer 4x4
        sprintf(print,"%i",breite);
        strcat(print, ", ");
        sprintf(print,"%i", hoehe);

        // fehlt: siehe Meilenstein1 Blatt ?? Funktion printFeld() wäre an der Stelle super!

        strcpy(print,"ENDFIELD");
        strcpy(print,"QUIT"); //beendet das Spiel (anscheinend: QUIT)


        if(response!=NULL){
          free(response);
        }
        response = NULL;

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
