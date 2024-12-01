#include "hop.h"

extern char home_dir[1024];
char prev_dir[1024];         // To store the previous directory

void hop(const char *path) {
    char curr_dir[1024];
    
    if (strcmp(path, "~") == 0 || strlen(path) == 0) {  // If path is "~" or empty, go to initial home directory
        if (chdir(home_dir) != 0) {
            perror("error");
        } else {
            if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
                printf("%s\n", curr_dir);
            } else {
                perror("error");
            }
        }
    }
    // Handle previous directory "-"
    else if (strcmp(path, "-") == 0) {
        if (strlen(prev_dir) == 0) {
            fprintf(stderr, "error: no previous directory to return to.\n");
            return;
        }
        if (chdir(prev_dir) != 0) {
            perror("error");
        } else {
            printf("%s\n", prev_dir);  // Print the new current directory after hopping
        }
    }

    else {
        // Save the current directory before changing
        if (getcwd(prev_dir, sizeof(prev_dir)) == NULL) {
            perror("error");
            return;
        }
        if (chdir(path) != 0) {
            perror("error");
        }
        else {
            if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
                printf("%s\n", curr_dir);
            } else {
                perror("error");
            }
        }
    }
}

