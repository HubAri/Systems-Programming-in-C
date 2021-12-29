#include "performConnection.h"
#include "handleRequest.h"

void performConnection(int fileSock) 
{
  
    char *buffer = (char*) malloc(BUFFERLENGTH*sizeof(char));
    
    
    //hier Überwachung aller Aufgaben und ankommender Dinge
    do{ 
      FD_ZERO(&readfd);                                                          //Macht das Set frei
      FD_SET(fileSock, &readfd);                                                     //Fügt dem Set den Socket hinzu (die Gameserververbindung)
      FD_SET(fd[0], &readfd);                                                    //Fügt dem Set die Pipe hinzu (Leseseite!)

      tv.tv_sec = 1;                                                              //Sekunden
      tv.tv_usec = 0;   
      
      retval = select(sizeof(readfd)*2, &readfd, NULL, NULL, &tv);     
      if(retval == -1){
        perror("select()");
        exit(EXIT_FAILURE);
      }else if(retval){
        pipeData = FD_ISSET(fd[0], &readfd);                                     //ISSET testet, ob an DIESER PIPE etwas ansteht
        socketData = FD_ISSET(fileSock, &readfd); 
        
        if(socketData != 0){  
          if((read(fileSock, buffer, BUFFERLENGTH)) < 0){                             //Lese nächsten Spielzug aus der Pipe
          perror("Host is not responding");                                     //Error, wenn aus der Pipe nicht gelesen werden konnte
        }
        processAndSendResponse(buffer, fileSock);
        }
      }

      if(pipeData!=0){                                                     //Wenn etwas ansteht, dann..Aus der Pipe lesen
        char *move = calloc(BUFFERLENGTH_MOVE,sizeof(char));
        if((read(fd[0], move, BUFFERLENGTH_MOVE)) < 0){                         //Lese nächsten Spielzug aus der Pipe
          perror("Couldn't read from pipe");                                    //Error, wenn aus der Pipe nicht gelesen werden konnte
        } 
      }                                                    
   
      //springen heraus sobald wir "Quit" erreichen
    }while(end);               
    
    
    

    
}

void sendResponse(char *response, int fileSock){
  if (response!=NULL){                                                          //Wenn es eine Anfrage gibt
    send(fileSock,response,strlen(response),0);                                     //Sendet dem Server die Antwort des Clients
    printf("client: %s",response);                                              //Gesendete Antwort ausgeben
  }
  if(response!=NULL){                                                           //Speicher von response freigeben da, in handle Speicher allokalisiert wurde
      free(response);                                                           //Response referenziert auf diesen Speicher
  }

}

void processAndSendResponse( char *buffer, int fileSock){
  
  char **requests = (char**)  malloc(REQUESTSLENGTH*sizeof(char*));
  //in recv_all() wird recv() so oft aufgerufen, bis wir ein \n als letztes Zeichen haben
  int line_length; 
  line_length = recv_all(fileSock, buffer, BUFFERLENGTH-1);
  buffer[line_length] = '\0'; 

  //in einem Buffer können sich meherere Linien befinden, die strenne wir mit stringToken()
  stringToken(buffer, "\n",requests);  

  //counter
  int c = 0;                                                             

  do{
  //zum Prüfen ob Ende der Prologphase erreicht (match() gibt bei erfolgreichem match 1 zurück.)
    end = !match(requests[c]+2,"QUIT");     

    //Server schickt positive Nachricht                       
    if(buffer[0]=='+'){   

      //Prüfen, dass Nachricht nicht bloß aus + besteht                                         
      if(strlen(requests[c])>2){ 

      //Servernachricht ausgeben
      //printf("S: %s\n",(requests[c]+2));

      //Antwort generieren  
      char *response = handle(requests[c]+2);
      
          
    //Ist die Antwort leer, dann springen wir zur nächsten Servernachricht
     if(response == NULL) {
       c++;
       continue;
     }
     sendResponse(response, fileSock);
     /*
     //Antwort an Server schicken
     send(fileSock,response,strlen(response),0);                        
            
     //Gesendete Antwort ausgeben
     printf("C: %s",response);
            
     //den Speicher von response freigeben  
                                                   
     free(response);  */
    }   
                                                    
  } //Negative Nachricht vom Server erhalten 
  else if(buffer[0]=='-'){
 //Fehlermeldung ausgeben
  printf("S: Error! %s\nDisconnecting server...\n",buffer+2);
  //Speicher freigeben
  free(buffer);
  free(requests[0]);
  free(requests);
  //return;                                                   
  }
  //Zähler inkrementieren
  c++;
  //solange es neue Lines gibt und wir "ENDPLAYERS" nicht erreicht haben bleiben wir in der Schleife
 } while(requests[c]!=NULL && end);
     
  free(requests[0]);

  free(requests);
  free(buffer);
}

void sendMove(char *move, int fileSock){
  char *response = calloc(BUFFERLENGTH_MOVE+strlen(move),sizeof(char));
  //memset(response, 0, strlen(move));
  strcpy(response, "PLAY ");
  strcat(response,move);
  strcat(response,"\n");
  sendResponse(response, fileSock);
  free(move);

}