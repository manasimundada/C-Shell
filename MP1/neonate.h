#ifndef NEONATE_H
#define NEONATE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

// Function declarations
pid_t get_recent_process_pid();  // Get the most recently created process ID
int kbhit();  // Check if a key was pressed (non-blocking)
void neonate_command(int time_arg);  // Run the neonate command

#endif // NEONATE_H
