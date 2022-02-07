
#include "handleRequest.h"
#include "prolog.h"
#include "header.h"
#include "handleResponse.h"
#include "thinker.h"



//Variables from the game phase:
int moveTime;
int board[4][4];
bool player0won = false;
bool player1won = false;
char winnerName[126];
int nextPiece;
int nextSquare;
int nextOpponentPiece;
int freePieces[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
char nextMove[16];
char nextCoordinates[2];
int free_squares[16];
int height;
int width;


bool game(int socket_fd) {

  //declare variables for select and initialize them
  int ready_for_reading;
  fd_set readset;
  fd_set readset_copy;
  struct timeval tv;
  FD_ZERO(&readset);
  

  int fdmax;
  tv.tv_sec = 0; 
  tv.tv_usec = 500000;

  //we need the largest file descriptor for the select method
  
  if(socket_fd > pfds[0]) fdmax = socket_fd;
  else fdmax = pfds[0];
  
  


  char *line = (char*) malloc(BUFFERLENGTH*sizeof(char));
  char msg[64];
  

  /* Each phase of the game has a number for the switch:
  default (and "Idle") -> 0
  "Move" -> 1 (name from script is somewhat misleading as we actually receive the board)
  "Game Over" -> 2
  "Make move" -> 3
  */
  int phase = 0;

  //flag needed for phase 3()
  bool skipReading = false; 

  while(true) {

    FD_ZERO(&readset_copy);
    FD_SET(pfds[0], &readset);
    FD_SET(socket_fd, &readset);
    
    if(!skipReading) {

      if(!read_line(socket_fd, line)) {
        perror("reading line");
        return false;
      }

      //check if message is negative
      if(line[0] == '-') {
        printf("S: Error! %s\nC: Disconnecting server...\n",line+2);
        free(line);
        return false;
      } 
    }

    switch(phase) {

      //default and "Idle"
      case 0:

        if(match(line + 2, "^MOVE .+$")) {

          sscanf(line + 2, "MOVE %d", &moveTime);
          phase = 1;
          break;
        }

        if(match(line + 2, "^WAIT$")) {

          printf("S: %s\n", line + 2);
          strcpy(msg,"OKWAIT\n");

          if(write(socket_fd, msg, strlen(msg)) == -1) {
            perror("sending msg to server");
            return false;
          }

          printf("C: OKWAIT\n");
          break;
        } 

        if(match(line + 2, "^GAMEOVER$")) {
          phase = 2;
          break;
        }

        //should not reach this point
        perror("unexpected message from server");
        free(line);
        return false;

      //"Move"  
      case 1:

        if(match(line + 2, "NEXT .+")) {
          sscanf(line + 2, "NEXT %d", &nextPiece);
          break;
        }

        if(match(line + 2, "^FIELD ?,?")) {
          sscanf(line + 2, "FIELD %d,%d", &width, &height);
          break;
        } 

        if(match(line + 2, "^ENDFIELD$")) {
          strcpy(msg, "THINKING\n");
          serverinfo->calcFlag = 1;
          if(write(socket_fd, msg, strlen(msg)) == -1) {
            perror("sending msg to server");
            return false;
          }
            //the first time we receive the FIELD width and height we create a shared memory segment
          if(shmID_board == -1) {

          
            /*
            board segment consists of one int height, one int width, one int nextPiece
            and a matrix int board[height][width], i.e.

            _____________________________________________________________________________________
            | height | width | nextPiece | board[0][0] | board[0][1] |...| board[height][width] |
            ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
            */ 
            shmID_board = creatingSHM((width*height + 3)*sizeof(int));

            //copy the segment ID into the shared memory that already exists
            serverinfo->shm_identifier = shmID_board;

            //attach process to segment
            shm_board_address = attachingSHM(shmID_board);

          }
          shm_board_address[0] = height;
          shm_board_address[1] = width;
          shm_board_address[2] = nextPiece;
          //store board in shared memory
          for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
        
              shm_board_address[4*i + j + 3] = board[i][j];
            }
          }

          kill(serverinfo->thinker, SIGUSR1);
          break;
        }


        if(match(line + 2, "OKTHINK")) {

          /*
         the select method pays attention 100 microseconds to the pipe and then 100 microseconds
         to the server socket and then back to the pipe and so on.
         */
          while(true) {
            
            ready_for_reading = select(fdmax + 1, &readset, NULL, NULL, &tv);
            
            
            //check for errors
            if(ready_for_reading < -1) {
              perror("select after sending OKTHINK failed");
              return false;

            //check if the server send a Timeout message
            } else if(ready_for_reading > 0 && FD_ISSET(socket_fd, &readset)) {
              
              //read what the server send
              if(!read_line(socket_fd, line)) {
                perror("reading line");
                return false;
              }

              //check if the message is negative
              if(line[0] == '-') {
                printf("S: Error! %s\nC: Disconnecting server...\n",line+2);
                free(line);
                return false;
              }


            //check if thinker responded through pipe
            } else if (ready_for_reading > 0 && FD_ISSET(pfds[0], &readset)) {
              //read the pipes message
              read(pfds[0], nextMove, 16);
              phase = 3;
              skipReading = true;
              break;

            } //end of if branch within while loop
          } //end of while loop
          
        }//end of OKTHINK if-case


        //we need this additional break because of the new while loop above
        if(phase == 3) break;
         
        
        recv_board(line + 2);
        break;

      //Gameover:
      case 2:

        if(match(line + 2, "^PLAYER0WON .+")) {
          
          if(strcmp(line + 2, "PLAYER0WON Yes") == 0) {
            player0won = true;
          }
          break;
        }

        if(match(line + 2, "^PLAYER1WON .+")) {
          
          if(strcmp(line + 2, "PLAYER1WON Yes") == 0) {
            player1won = true;
          }
          break;
        }

        if(match(line + 2, "^QUIT$")) {
          
          puts("\n ---------------------------");
          if(player0won) { 
            printf("| Player 1 has won the game |\n");
          } else if (player1won) {
            printf("| Player 2 has won the game |\n");
          } else {
            printf("| Its a tie                 |\n");
          }
          puts(" ---------------------------");
          free(line);
          return true;
        }

        if(match(line + 2, "^FIELD .,.")) {
          sscanf(line + 2, "FIELD %d,%d", &width, &height);
          break;
        } 

        if(match(line + 2, "^ENDFIELD$")) {

            //create a shared memory the first time we receive the FIELD message
          if(shmID_board == -1) {

            //segment consists of one int height, one int width, one int nextPiece and a matrix int board[height][width] 
            shmID_board = creatingSHM((width*height + 3)*sizeof(int));

            //copy the segment ID into the shared memory that already exists
            serverinfo->shm_identifier = shmID_board;

            //attach process to segment
            shm_board_address = attachingSHM(shmID_board);

          }
          shm_board_address[0] = height;
          shm_board_address[1] = width;
          shm_board_address[2] = nextSquare;
          //store board in shared memory
          for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
        
              shm_board_address[4*i + j + 3] = board[i][j];
            }
          }
          print_board_binary(height, width, board);
          break;
        }

        recv_board(line + 2);
        break;

      //Make move:  
      case 3:
        if(skipReading) {
          
          
          printf("nextMove: %s\n", nextMove);

          if(write(socket_fd, nextMove, strlen(nextMove))== -1) {
            perror("sending msg to server");
            return false;
          }

          skipReading = false;
        } else if(match(line + 2, "^MOVEOK$")) {
          phase = 0;
          break;

        } else {
          perror("unexpected message from server");
          return false;
        }
      
      default:
      break;
    }    
  }
}


bool recv_board(char *line) {
  int temp = atoi(&line[0]) - 1;
  char array[4][16];
  sscanf(line + 2, "%s %s %s %s", &array[0][0], &array[1][0], &array[2][0], &array[3][0]);
  for(int j = 0; j < 4; j++) {
    if(array[j][0] == '*') {
      board[temp][j] = -1;
    } else {
      int temp2 = atoi(&array[j][0]);
      board[temp][j] = temp2;
      freePieces[temp2] = -1;
    }
  }
  
  return true;
}
