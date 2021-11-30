CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
SRCFILES = sysprak-client.c helperFunctions.c serverConnection.c performConnection.c handleResponse.c handleRequest.c
MAIN = sysprak-client

all: $(MAIN) 

$(MAIN): $(SRCFILES)
	$(CC) $(CFLAGS) -o $(MAIN) $(SRCFILES)
    
clean:
	rm -f $(MAIN)