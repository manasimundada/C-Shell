#ifndef IOREDIR_H
#define IOREDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_TOKENS 1024

// Function prototypes
void handle_io_redirection(char *cmd);
int handle_redirection(char **tokens);


#endif /* IOREDIR_H */
