# Quarto4fun
Systempraktikum WiSe 2021/22 - Group 19 

Description: This project realizes a client, that can play the game quarto. The main function is inside sysprak-client.c. Initial tasks are implemented in init.c. Afterwards a TCP connection is established to the server with code from connect.c. Then the prolog phase beginns from within prolog.c and  consequently the actual game is played with code implemented in game.c. (notice, that we have two files, prolog.c and game.c instead of the file performConnection.c)

Additionally, there are several header files and some files containing helper functions.

Authors: Lorena Wemmer, Arian Moharramzadeh, Tobias Rohe, Isabella Debelic und Alexander Stadelmann

Tutor: Rufus Maiwald

# How To Use

call: 
```
./sysprak-client with two parameters: -g <GAME_ID> <br/>
Additionally you may add:  -p <PLAYER_NUMBER> or -c <config_file>
```
NOTE: Need to connect to LMU gameserver (via VPN) to access frontend / game in browser. Client can then be connected.
