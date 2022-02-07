#include "handleRequest.h"


int match(const char *string, char *pattern)
{
    int result;
    regex_t reg;

    if (regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB) != 0)
        return 0;
    result = regexec(&reg, string, (size_t) 0, NULL, 0);                         
    regfree(&reg);                                                               
    if (result != 0) {                                                         
        return(0);                                                              
    }
    return(1);        
}        

char *substring(char *string, unsigned int from, unsigned int to){
  if(from>=to || to>strlen(string)){                                            
    return NULL;                                                                
  }else{                                                                        
    char *substring = malloc((to-from)*sizeof(char)+1);                         
    strncpy(substring,string+from,(to-from));                                   
    substring[(to-from)]= '\0';                                                 
    return substring;                                                           
  }
}

int stringToken(char *str, char *divider, char **requests){
  int i = 0;
  char *entry = (char*) malloc(BUFFERLENGTH*sizeof(char));
  strcpy(entry, str);
  requests[0] = strtok(entry, divider);                               
  while (requests[i]){                              
    i++;                                                   
    requests[i] = strtok(NULL, divider);                               
  }
  return i;
}
