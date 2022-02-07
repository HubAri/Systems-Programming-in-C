#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "handleRequest.h"
#include "handleResponse.h"


bool prolog(int);
bool read_line(int, char*);
bool game(int);
