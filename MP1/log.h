#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

void add_log(const char *home, const char *command);
void print_log(const char *home);
void purge(const char *home);
void exec_log(int number, const char* home);

#endif