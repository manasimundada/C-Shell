#ifndef PROMPT_H
#define PROMPT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

char* get_user();
char* get_host();
void print_prompt(char *home_dir);

#endif
