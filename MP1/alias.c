#include "alias.h"
// Function prototypes
void mk_hop(char *dir);
void hop_seek(char *name);

// Alias definitions
char *aliases[][2] = {
    {"reveall", "reveal -l"},
    {"reveala", "reveal -a"},
    {"home", "hop ~"},
    // Add more aliases here
};

int num_aliases = sizeof(aliases) / sizeof(aliases[0]);

// Function to execute an alias
void execute_alias(const char *alias) {
    for (int i = 0; i < num_aliases; i++) {
        if (strcmp(alias, aliases[i][0]) == 0) {
            system(aliases[i][1]);
            return;
        }
    }
    printf("Alias not found: %s\n", alias);
}

// BONUS functions
void mk_hop(char *dir) {
    char command[256];
    snprintf(command, sizeof(command), "mkdir \"%s\" && hop \"%s\"", dir, dir);
    system(command);
}

void hop_seek(char *name) {
    char command[256];
    snprintf(command, sizeof(command), "hop \"%s\" && seek \"%s\"", name, name);
    system(command);
}