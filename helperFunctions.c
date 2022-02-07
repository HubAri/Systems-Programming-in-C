#include "header.h"


bool is_valid_id(char *arg) 
{
  int i = 0;
  int length = 0;
  while(arg[i] != '\0') {
    if(!isalnum((int)(arg[i]))) return false;
    i++;
    length++;
  } 
  if(length != 13) return false;
  return true;
}

bool is_valid_player_number(char *arg) 
{
  if((arg[0]== '1' || arg[0]== '2') && arg[1]== '\0') {
    return true;
  } else {
    return false;
  }
}

bool is_valid_file(char *arg, char *string)
{
  if(strcmp(arg,string) == 0){
    return false;
  } else {
    return true;
  }

}



void print_id(char *ptr, int length)
{
  for(int i = 0; i < length; i++){
    printf("%d", ptr[i]);
  }
}

int recv_all(int sockfd, char *buffer, size_t len)
{
  
  int packet_length = 0;
  int temp;
  do{
    temp = read(sockfd, buffer+packet_length, len);
    packet_length += temp;
  } while(buffer[packet_length - 1] != '\n');
  return packet_length;
}

bool read_line(int socket_fd, char* ptr) {
  int len = 0;
  do {

    if((read(socket_fd, ptr + len, 1) == -1)) {
      return false;
    } len++;

  } while (ptr[len - 1] != '\n');
  ptr[len - 1] = '\0';
  return true;
}


void print_board(int height, int width, int board[height][width]) {
  
  printf("\nNext: %d\n", nextPiece);
  puts("    A  B  C  D ");
  puts("   ------------");
  for(int i = height - 1; i >= 0; i--) {
    printf("%d |", i + 1);
    for(int j = 0; j < width; j++) {
      if(board[i][j] == -1) {
        printf(" * ");
      } else if(board[i][j] < 10) {
        printf(" %d ", board[i][j]);
      } else{
        printf(" %d",board[i][j]);
      }
      
    }
    printf("|\n");
  }
  puts("   ------------");
  puts("Remainig pieces: ");
  for(int i = 0; i < 16; i++) {
    if(freePieces[i] == -1) {
      printf(" * ");
    } else {
      printf("%d ", freePieces[i]);
    }
  }
  puts("");
}
char *intToBinary(int number, int length ) {
    
    int cursor = length;
  
    //allocate length many bytes plus one byte for \0
    char * result = (char *) malloc((length + 1)*sizeof(char));
    result[cursor] = '\0';
    cursor--;
    while (number > 0) {
        if (number % 2 == 0) {
            *(result + cursor) = '0';
        }
        else {
            *(result + cursor) = '1';
        }
        number /= 2;
        cursor--;
    }
    while(cursor >= 0) {
      result[cursor] = '0';
      cursor--;
    }
    return result;
}

void print_cube() {
  for(int i = 0; i < 4;i++) {
    printf("|");
    for(int j = 0; j < 4; j++) {
      printf("%s ", cube[i][j]);
    } 
    puts("");
  }
  puts("|");
}

void print_board_binary(int height, int width, int board[height][width]) {
  
  printf("\nNext: %d\n", nextPiece);
  puts("     A     B     C     D   ");
  puts("   ------------------------");
  for(int i = height - 1; i >= 0; i--) {
    printf("%d |", i + 1);
    for(int j = 0; j < width; j++) {
      if(board[i][j] == -1) {
        printf("  *   ");
      } else {
        char *temp = intToBinary(board[i][j], 4);
        printf("%s  ", temp);
        free(temp);
      }
      
    }
    printf("|\n");
  }
  puts("   ------------------------");
  puts("Remainig pieces: ");
  for(int i = 0; i < 16; i++) {
    if(freePieces[i] == -1) {
      printf(" * ");
    } else {
      printf("%d ", freePieces[i]);
    }
  }
  puts("");
}