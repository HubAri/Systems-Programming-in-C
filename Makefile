CC = gcc
CFLAGS = -Werror -Wall -Wextra  -g
SRCFILES = sysprak-client.c helperFunctions.c connect.c handleRequest.c config.c sharedMemoryFunctions.c prolog.c game.c thinker.c init.c
MAIN = sysprak-client

all: $(MAIN) 

$(MAIN): $(SRCFILES)
	$(CC) $(CFLAGS) -o $(MAIN) $(SRCFILES)
    
clean:
	rm -f $(MAIN)
