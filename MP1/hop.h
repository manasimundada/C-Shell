#ifndef HOP_H
#define HOP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

extern char home_dir[1024]; 
void hop(const char* path);

#endif