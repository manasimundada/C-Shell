#ifndef REVEAL_H
#define REVEAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

void reveal(const char *path, int show_details, int show_all);
void reveal_l(const char *path, const struct stat *mystat);

#endif