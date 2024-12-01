#ifndef SEEK_H
#define SEEK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "reveal.h"

void print_colored(const char* path, bool is_dir);
char* resolve_path(const char* path);
void search_directory(const char* base_path, const char* dir_path, const char* target, bool only_dirs, bool only_files, bool exec_flag, int* match_count);

#endif // SEEK_H
