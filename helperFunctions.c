#include "header.h"

bool is_valid_id(char *arg) {
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

bool is_valid_player_number(char *arg) {
  if((arg[0]== '1' || arg[0]== '2') && arg[1]== '\0') {
    return true;
  } else {
    return false;
  }
}

void id_init(char *ptr, int size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    int len = (int) sizeof charset - 1;
    if (size) {
      --size;
      for (int i = 0; i < size; i++) {
        int key = rand() % (int) len;
        ptr[i] = charset[key];
      }
      ptr[size] = '\0';
    }
}

void print_id(char *ptr, int length) {
  for(int i = 0; i < length; i++){
    printf("%d", ptr[i]);
  }
}