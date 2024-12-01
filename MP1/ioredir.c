#include "ioredir.h"
#include "tokenise.h"

// Function to handle input redirection
void input_redir(char *input_file) {
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1) {
        fprintf(stderr, "Error: No such input file found!\n");
        return;
    }
    dup2(input_fd, STDIN_FILENO); // Redirect standard input
    close(input_fd);
}

// Function to handle output redirection
void output_redir(char *output_file, int append) {
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int output_fd = open(output_file, flags, 0644);
    if (output_fd == -1) {
        perror("Error opening output file");
        return;
    }
    dup2(output_fd, STDOUT_FILENO); // Redirect standard output
    close(output_fd);
}

// Function to handle I/O redirection
void handle_io_redirection(char *cmd) {
    char *demarcator_in = NULL;
    char *demarcator_out = NULL;
    char temp_input[256]; // Temporary string to work with
    char output_file[256];
    char input_file[256];
    int append = 0;
    int both_present = 0; // Flag for both < and >
    
    strcpy(temp_input, cmd);
    strcpy(temp_input, cmd);
    int og_in = dup(STDIN_FILENO);
    int og_out = dup(STDOUT_FILENO);

    // Check for both "<" and ">" in the input string
    demarcator_in = strchr(temp_input, '<');
    demarcator_out = strstr(temp_input, ">>");  // Check for ">>" first
    if (demarcator_out == NULL) {
        demarcator_out = strchr(temp_input, '>');  // Check for ">"
    }

    // If both are present
    if (demarcator_in != NULL && demarcator_out != NULL) {
        both_present = 1;
    }

    // If both < and > are present
    if (both_present) {
        // Handle input redirection first
        *demarcator_in = '\0'; // Split the string at "<"
        demarcator_in++;
        
        // Now get the substring between '<' and '>'
        char *end_of_input = strchr(demarcator_in, '>');  // Find the '>'
        if (end_of_input != NULL) {
            *end_of_input = '\0';  // Null terminate at '>'
        }
        
        strcpy(input_file, trim_whitespace(demarcator_in));

        // Then handle output redirection
        demarcator_out++;  // Move past ">" (or ">>" will be handled below)
        strcpy(output_file, trim_whitespace(demarcator_out));

        // Redirect input first
        input_redir(input_file);
        
        // Redirect output
        append = (strstr(cmd, ">>") != NULL) ? 1 : 0;  // If ">>" is found, set append
        output_redir(output_file, append);

    } else if (demarcator_out != NULL) {  // Handle only output redirection
        *demarcator_out = '\0'; // Split the string at ">" or ">>"
        demarcator_out += (strstr(cmd, ">>") != NULL) ? 2 : 1; // Move past ">>" or ">"
        strcpy(output_file, trim_whitespace(demarcator_out));
        append = (strstr(cmd, ">>") != NULL) ? 1 : 0;  // If ">>" is found, set append
        output_redir(output_file, append);

    } else if (demarcator_in != NULL) {  // Handle only input redirection
        *demarcator_in = '\0'; // Split the string at "<"
        demarcator_in++;
        strcpy(input_file, trim_whitespace(demarcator_in));
        input_redir(input_file);
    }

    // Remove the redirection part from the command string
    strcpy(cmd, trim_whitespace(temp_input));

    // Tokenize and execute the command
    char *tokens[MAX_TOKENS];
    int token_count = 0;
    char *token = strtok(cmd, " \t");
    
    while (token != NULL && token_count < MAX_TOKENS - 1) {
        tokens[token_count++] = token;
        token = strtok(NULL, " \t");
    }
    tokens[token_count] = NULL;

    // Execute the command without redirection symbols
    check_and_execute_special_command(tokens, token_count, 0);

    // Restore the original file descriptors
    fflush(stdout);
    dup2(og_in, STDIN_FILENO);
    dup2(og_out, STDOUT_FILENO);
    close(og_in);
    close(og_out);
}
