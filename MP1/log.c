#include "log.h"
#include "tokenise.h"

// Function to update the log file with a new command
void add_log(const char *home, const char *command) {
    // Do not store if the command contains the word "log"
    if (strstr(command, "log") != NULL) {
        return;
    }

    // Construct the log file path
    char log_path[256];  // Ensure this is large enough to hold the home directory path and "log.txt"
    snprintf(log_path, sizeof(log_path), "%s/log.txt", home);

    // Open the log file in read mode to read existing commands
    FILE *log_file = fopen(log_path, "r");
    if (log_file == NULL) {
        // If file doesn't exist, open it in write mode to create
        log_file = fopen(log_path, "w");
        if (log_file == NULL) {
            perror("Error creating log file");
            return;
        }
        fclose(log_file);
        log_file = fopen(log_path, "r");
    }

    // Read existing commands into a buffer
    char *commands[15] = {NULL};
    int command_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), log_file) && command_count < 15) {
        // Remove the newline character from the line if it exists
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines
        if (line[0] != '\0') {
            commands[command_count++] = strdup(line);
        }
    }

    // Check if the new command is the same as the last command in the buffer
    if (command_count > 0 && strcmp(commands[command_count - 1], command) == 0) {
        // The new command is a duplicate of the last command; do not add it
        fclose(log_file);
        return;
    }

    fclose(log_file);

    // Add the new command to the buffer
    if (command_count == 15) {
        free(commands[0]);
        for (int i = 1; i < 15; i++) {
            commands[i - 1] = commands[i];
        }
        commands[14] = strdup(command);
    } else {
        commands[command_count++] = strdup(command);
    }

    // Open the log file in write mode to rewrite the updated commands
    log_file = fopen(log_path, "w");
    if (log_file == NULL) {
        perror("Error opening log file for writing");
        return;
    }

    // Write the updated list back to the log file
    for (int i = 0; i < command_count; i++) {
        fprintf(log_file, "%s\n", commands[i]);
        free(commands[i]); // Free each line after writing
    }

    fclose(log_file);
}

// Function to print the log file contents
void print_log(const char *home) {
    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/log.txt", home);
    FILE *log_file = fopen(log_path, "r");
    if (log_file == NULL) {
        perror("Error opening log file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), log_file)) {
        printf("%s", line);
    }

    fclose(log_file);
}

// Function to clear the log file
void purge(const char *home) {
    char log_path[256];
    snprintf(log_path, sizeof(log_path), "%s/log.txt", home);

    FILE *log_file = fopen(log_path, "w"); // Open in write mode to clear the file
    if (log_file != NULL) {
        fclose(log_file);
    }
}

void exec_log(int number,const char* home){
    char log_path[256];  // Ensure this is large enough to hold the home directory path and "log.txt"
    snprintf(log_path, sizeof(log_path), "%s/log.txt", home);

    // Open the log file in read mode to read existing commands
    FILE *log_file = fopen(log_path, "r");
    if (log_file == NULL) {
        // If file doesn't exist, open it in write mode to create
        log_file = fopen(log_path, "w");
        if (log_file == NULL) {
            perror("Error creating log file");
            return;
        }
        fclose(log_file);
        log_file = fopen(log_path, "r");
    }
    
    char *commands[15] = {NULL};
    int command_count = 0;
    char line[256];

    while (fgets(line, sizeof(line), log_file) && command_count < 15) {
        // Remove the newline character from the line if it exists
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines
        if (line[0] != '\0') {
            commands[command_count++] = strdup(line);
        }
    }

    fclose(log_file);
    if(command_count-(number)<0){
        perror("not enough history logged\n");
        return;
    }
    char* input = strdup(commands[command_count-(number)]);
    execute_commands(input, 1);
}