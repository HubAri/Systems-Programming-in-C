#ifndef reqHan
#define reqHan


#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"

int result;

int match(const char *string, char *pattern);
char *substring(char *string, unsigned int from, unsigned int to);              
int stringToken(char *str, char *divider, char *token[]); 



#endif