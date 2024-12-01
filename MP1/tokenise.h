#ifndef TOKENISE_H
#define TOKENISE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

char *trim_whitespace(char *str);
void check_background_processes();
void execute_command(char **args, int background);
void check_and_execute_special_command(char **tokens, int token_count, int background);
void add_process(pid_t pid, const char *command, int background);
void sigchld_handler(int signo);
void initialize_signal_handlers();
void handle_background_processes();
void execute_commands(char *input, int flag);
void check_background_processes();

#endif