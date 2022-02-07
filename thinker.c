
#include "thinker.h"

/*
The following functions rely on a handful of global variables defined in game.c
*/


/*
  input: matrix of ints with given width and height and an array of ints of length width*height
  output: number of free pieces
  */
int free_pieces_search(int height, int width, int matrix[height][width], int free_pieces[]) {
  int count = height*width;

  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {

      if(matrix[i][j] != -1) {
        free_pieces[matrix[i][j]] = -1;
        count--;
      }
    }
  }
  return count;
}



/*
calculateMove is an umbrella function that encompasses all steps
needed to find a move to send to the connector.
*/
void calculateMove() {

  //stores the resulting array in freePieces
  free_pieces_search(height, width, board, freePieces);
  
  //stores the resulting array in free_squares and returns the number of free squares
  int square_options = free_squares_search(height, width, board, free_squares);
  
  nextSquare = find_winning_move(nextPiece, height, width, board,square_options, free_squares);
  if(nextSquare == -1) {

    //seed the rand() function
    srand(time(0));
    //choose a random square
    nextSquare = free_squares[rand() % square_options];
  }
  
  

  convert_coordinates(height, width, nextSquare, nextCoordinates);

  insertNextMove(nextCoordinates, nextMove);

}

/*Update the free_squares array so that it contains all the available fields.
It returns the number of available fields, i.e. the length of the array.
The free_squares array must have the same length as height*width of the matrix.
*/
int free_squares_search(int height, int width, int matrix[height][width], int free_squares[]) {
  int count = 0;
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      
      if(matrix[i][j] == -1) {
        
        free_squares[count] = i*width + j;
        count++;
      }
    }
  }
  return count;
}

//translates the standard coordinates (0 to 15) to coordinates like "A4";
//the result is stored in next_coordinates[2];
void convert_coordinates(int height, int width, int pos, char next_coordinates[]) {

  char column = (char) (pos % width + 65);
  char row = (char) (pos / height + 49);
  next_coordinates[0] = column;
  next_coordinates[1] = row;
}

//create the message that the client sends the server, e.g  "PLAY B2,7"
void insertNextMove(char next_coordinates[], char next_move[]) {
  strcpy(next_move, "PLAY ");
  strcat(next_move, next_coordinates);
  chooseNextOpponentPiece(height*width, freePieces);
  char temp[4];
  bool winningMove = is_winning_move(nextPiece, nextSquare, height, width, board);

  /*this condition checks two things:
   1) is this the last move, i.e. the move that results in the board being full afterwards?
   2) is this a "winner"-move?
  */
  if(nextOpponentPiece != -1 && !winningMove) {
    sprintf(temp, ",%d\n", nextOpponentPiece);
    strcat(nextMove, temp);
  } else {
    strcat(nextMove, "\n");
  }
  
}

void chooseNextOpponentPiece(int length, int free_pieces[]) {
  int cursor = 0;
  while(free_pieces[cursor] == -1) {
    cursor++;
  }
  if(cursor <= length) {
    nextOpponentPiece = cursor;
  } else {
    nextOpponentPiece = -1;
  }
}

/*is_winning_move() uses "bitwise and" to check if four pieces in a line
have a quality in common stored as a "1" in the binary representation of the piece.
The lines can be horizonal, vertical or diagonal.
If four pieces in a line do not share quality stored as "1", then the result
of the four "bitwise and's" is zero.

Also we have to check qualities stored as "0" in binary. 
For that purpose we invert the binaries(the piece and the board)
and proceed like before.
*/
bool is_winning_move(int piece, int square, int height, int width, int board[height][width]) {
  //check for common qualities stored as "1"s
  if(is_winning_move_helper(piece, square, height, width, board)) return true;
  int board_copy[height][width];
  memcpy(board_copy, board, width*height*sizeof(int));
  //invert binaries to check for common qualities stored as "0"s
  piece ^= 15;

  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++){
      
      if(board_copy[i][j] != -1) {
        board_copy[i][j] ^= 15;
      }
    } 
  }

  //call helper function with inverted binaries
  return is_winning_move_helper(piece, square, height, width, board_copy);

}

//helper function 
bool is_winning_move_helper(int piece, int square, int height, int width, int board[height][width]) {
    int res = piece;
    int column = square % 4;
    int row = square / 4;
  
  //check vertical lines
  for(int i = 0; i < height; i++) {

    //skip the Square where we want to position the piece
    if(i == row) continue;

    //if a Square is empty, then we know its not a winning move
    if(board[i][column] == -1) {
      res = 0;
      break;
    }
      //use bitwise and
      res &= board[i][column];
  }
  //if the result is not zero we have a winner
  if(res != 0) {
    return true;
  } 

  //restore initial value of res
  res = piece;
  


  //check horizontal lines like above
  for(int j = 0; j < width; j++) {

    if(j == column) continue;

    if(board[row][j] == -1) {
      res = 0;
      break;
    }

    res = res & board[row][j];
    
    
  }
  
  if(res != 0) {
    return true;
  } 

  res = piece;
    

  //diagonal from upper left corner to lower right corner
  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      
      //the sum of the indexes of the Squares on this diagonal equals three
      if(i + j == 3) {
        if(i == row && j == column) continue;

        if(board[i][j] == -1){
          res = 0;
          break;
        } 
        
        res = res & board[i][j];
      }
    } 
  }

  if(res != 0) {
    return true;
  } 

  res = piece;
    

  //diagonal line from lower left corner to upper right corner
  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      
      //row number equals column number on this diagonal
      if(i == j) {
        if(board[i][j] == -1) {
          res = 0;
          break;
        }
        
        res = res & board[i][j];
       
      }
    }
  }
  if(res != 0) {
    return true;
  } 

  return false;

}

//experimental: make binary cube
void make_cube_from_board(int height, int width, int board[height][width], int depth, int cube[height][width][depth]) {

  for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      int temp = board[i][j];
      for(int k = depth - 1; k >= 0; k--) {
        if(temp % 2 == 1) {
          cube[i][j][k] = 1;
        } else {
          cube[i][j][k] = 0;
        }
        temp /= 2;
      }
    }
  }
}

int find_winning_move(int nextPiece, int height, int width, int board[][width], int square_options, int free_squares[]) {
  for(int i = 0; i < square_options; i++) {
    if(is_winning_move(nextPiece, free_squares[i], height, width, board)) {
      puts("find_winning_move: success");
      return free_squares[i];
    }
  } 
  return -1;

}
