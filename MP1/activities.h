#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


struct Process {
    pid_t pid;
    char command[256];
    int running;
    int background;
};

extern struct Process processes[];
extern int process_count;

void activities_command();

#endif // ACTIVITIES_H
