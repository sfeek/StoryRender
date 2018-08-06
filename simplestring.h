#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


int safe_append(char **,char *);
int safe_copy(char **, char *);
int safe_sprintf(char **, char *fmt, ... );
int safe_truncate(char **, int);
