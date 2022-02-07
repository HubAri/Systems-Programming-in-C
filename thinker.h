#ifndef think
#define think

#include "header.h"

int free_squares[16];

bool is_winning_move(int, int, int height, int width, int[height][width]);
bool is_winning_move_helper(int, int, int height, int width, int[height][width]);
int find_winning_move(int, int, int, int board[][width], int, int[]);

int free_squares_search(int height, int width, int [height][width], int[]);

//Function from thinker.c
void calculateMove();
void chooseNextOpponentPiece();

void convert_coordinates(int, int, int, char[]);
void insertNextMove(char[], char[]);

#endif
