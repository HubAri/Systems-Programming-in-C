

//variables for command line arguments
char game_id[];
char player_number[];
char player_name[];
char cip_version[];
int player_check; 

//variables for config file
typedef struct{
    char hostName[BUFFERLENGTH];
    int portNumber;
    char gameKindName[BUFFERLENGTH];
}configparam;

//functions for command line arguments
bool check_argc(int);
bool get_args(int, char**);

//functions for config file
char* readConfig(char *name, char *config);
void createClientConfig(char *confile);
void make_config_file();