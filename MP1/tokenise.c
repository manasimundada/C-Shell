#include "prompt.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "tokenise.h"
#include "seek.h"
#include "alias.h"
#include "iMan.h"
#include "activities.h"
#include "ioredir.h"
#include "neonate.h"


#define MAX_PROCESSES 100
struct Process processes[MAX_PROCESSES];
int process_count = 0;

#define MAX_TOKENS 1024

// Global arrays to store tokens
char *ampersand_tokens[MAX_TOKENS];
char *semicolon_tokens[MAX_TOKENS];
char *space_tab_tokens[MAX_TOKENS];

// Global variables for directories
char home_dir[1024];
char prev_dir[1024];
char curr_dir[1024];

// Function to trim leading and trailing whitespace
char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    // Trim trailing space
    if (*str == 0)  // All spaces
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

// Function to add a process to the list
void add_process(pid_t pid, const char *command, int background) {
    if (process_count < MAX_PROCESSES) {
        processes[process_count].pid = pid;
        strncpy(processes[process_count].command, command, 255);
        processes[process_count].command[255] = '\0';
        processes[process_count].background = background;
        processes[process_count].running = 1;
        // printf("Debug: Added process PID %d, command '%s', background: %d\n", pid, command, background);
        // fflush(stdout);
        process_count++;
    }
}

// Modify the execute_command function
void execute_command(char **args, int background) {
    if (args[0] == NULL) return;  // Skip if no command to execute

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        if (background) {
            // Set the process group ID to itself to detach from shell's process group
            setpgid(0, 0);
        }
        execvp(args[0], args);
        printf("'%s' is not a valid command\n", args[0]);
        exit(1);
    } else {
        // Parent process: Add both foreground and background processes to the list
        add_process(pid, args[0], background);  // Add process to list

        if (background) {
            printf("[%d] %d\n", process_count, pid);
            printf("Running in background: PID %d\n", pid);
            // printf("Debug: Started background process PID %d\n", pid);
            // fflush(stdout);
        } else {
            // printf("Debug: Waiting for foreground process PID %d\n", pid);
            // fflush(stdout);
            // Foreground process: wait for it to finish
            int status;
            waitpid(pid, &status, WUNTRACED);  // Wait for foreground process to complete

            // After it finishes, update its status in the `processes` array
            for (int i = 0; i < process_count; i++) {
                if (processes[i].pid == pid) {
                    processes[i].running = 0;  // Mark process as not running
                    // printf("Debug: Foreground process PID %d completed\n", pid);
                    // fflush(stdout);
                    break;
                }
            }
        }
    }
}

// Add signal handler for SIGCHLD
void sigchld_handler(int signo) {
        // printf("Debug: sigchld_handler called\n");
    int status;
    pid_t pid;

    // Loop to handle all terminated background processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < process_count; i++) {
            if (processes[i].pid == pid && processes[i].background) {
                if (WIFEXITED(status)) {
                    printf("\n'%s' exited normally (PID %d)\n", 
                           processes[i].command, pid);
                } else if (WIFSIGNALED(status)) {
                    printf("\n'%s' exited abnormally with signal %d (PID %d)\n", 
                           processes[i].command, WTERMSIG(status), pid);
                }
                processes[i].running = 0;  // Mark process as not running
                fflush(stdout);
                break;
            }
        }
    }
}

void initialize_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("sigaction");
        exit(1);
    }
}

// Function to handle background process completion
void handle_background_processes() {
    int status;
    pid_t pid;

    // Check if any background process has completed
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < process_count; i++) {
            if (processes[i].pid == pid) {
                if (processes[i].background) {
                    // Check if the process exited normally
                    if (WIFEXITED(status)) {
                        printf("\n'%s' exited normally (PID %d)\n", 
                               processes[i].command, pid);
                    } else if (WIFSIGNALED(status)) {
                        // If the process was terminated by a signal
                        printf("\n'%s' exited abnormally with signal %d (PID %d)\n", 
                               processes[i].command, WTERMSIG(status), pid);
                    }
                }
                // Mark the process as not running
                processes[i].running = 0;
                fflush(stdout);
                break;
            }
        }
    }
}

// Function to check specific commands like 'hop', 'reveal', etc.
void check_and_execute_special_command(char **tokens, int token_count, int background) {
    if (token_count == 0) return;  // No command to process
    

    // if (handle_redirection(tokens) < 0) {
    //     return;  // Exit if redirection fails
    // }

    if (strcmp(tokens[0], "hop") == 0) {
        if (token_count == 1) {
            hop("~");
        } else {
            for (int i = 1; i < token_count; i++) {
                hop(tokens[i]);
            }
        }
    }
    else if (strcmp(tokens[0], "reveal") == 0) {
        int show_details = 0;
        int show_all = 0;
        char *paths[1024];
        int path_count = 0;

        for (int i = 1; i < token_count; i++) {
            if (tokens[i][0] == '-') {
                for (int j = 1; j < strlen(tokens[i]); j++) {
                    if (tokens[i][j] == 'l') show_details = 1;
                    else if (tokens[i][j] == 'a') show_all = 1;
                    else fprintf(stderr, "Unknown option '-%c'\n", tokens[i][j]);
                }
            } else {
                paths[path_count++] = tokens[i];
            }
        }

        if (path_count == 0) paths[path_count++] = "."; // Default to current directory

        for (int i = 0; i < path_count; i++) {
            reveal(paths[i], show_details, show_all);
        }
    }
    else if (strcmp(tokens[0], "log") == 0) {
        if (token_count == 1) {
            print_log(home_dir);
        } else if (strcmp(tokens[1], "purge") == 0) {
            purge(home_dir);
        }
        else if (strcmp(tokens[1], "execute") == 0) {
            if (token_count > 2){
                int exec_number = atoi(tokens[2]);
                exec_log(exec_number, home_dir);
            }
            else {printf("error: log execute requires a number.\n");}
        }
        else {printf("error: unknown log command '%s'.\n", tokens[1]);}
    }
    else if (strcmp(tokens[0], "proclore") == 0) {
        pid_t pid;
        if (token_count == 1) {
            pid = getpid();
        } else {
            pid = atoi(tokens[1]);
        }
        print_proclore(pid);
    }
    else if (strcmp(tokens[0], "seek") == 0) {
        if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
            // printf("Current working directory: %s\n", curr_dir);
        }
        else{perror("getcwd() error");}
            bool only_dirs = false;
        bool only_files = false;
        bool exec_flag = false;
        char* target = NULL;
        char* dir_path = ".";
        int opt_index = 1;
        // Parse flags
        while (opt_index < token_count && tokens[opt_index][0] == '-') {
            for (int j = 1; j < strlen(tokens[opt_index]); j++) {
                switch (tokens[opt_index][j]) {
                    case 'd':
                        only_dirs = true;
                        break;
                    case 'f':
                        only_files = true;
                        break;
                    case 'e':
                        exec_flag = true;
                        break;
                    default:
                        fprintf(stderr, "Unknown option '-%c'\n", tokens[opt_index][j]);
                        return;
                }
            }
            opt_index++;
        }
        if (only_dirs && only_files) {
            printf("Invalid flags!\n");
            return;
        }
        if ((!only_dirs && exec_flag) && (!only_files && exec_flag)) {
            printf("Error: need to specify a flag with -e\n");
            return;
        }
        // Parse target and directory
        if (opt_index < token_count) {
            target = tokens[opt_index++];
            if (opt_index < token_count) {
                dir_path = tokens[opt_index];
            }
        } else {
            fprintf(stderr, "Usage: seek [-d] [-f] [-e] <search> [<target_directory>]\n");
            return;
        }
        char* resolved_path = resolve_path(dir_path);
        int match_count = 0;

        search_directory(curr_dir, resolved_path, target, only_dirs, only_files, exec_flag, &match_count);
        if (match_count == 0) {
            printf("No match found!\n");
        }
        free(resolved_path);
    }
    else if (strcmp(tokens[0], "iMan") == 0) {
        handle_iman(tokens, token_count);
    }
    else if (strcmp(tokens[0], "activities") == 0) {
        activities_command();
    }
    else if (strcmp(tokens[0], "neonate") == 0) {
        if (token_count == 3 && strcmp(tokens[1], "-n") == 0) {
            int time_arg = atoi(tokens[2]);  // Convert the time_arg from string to integer
            if (time_arg > 0) {
                neonate_command(time_arg);   // Call neonate_command with time_arg
            } else {
                printf("Invalid time argument. Usage: neonate -n [time_arg]\n");
            }
        } else {
            printf("Usage: neonate -n [time_arg]\n");
        }
    }
    else if (strcmp(tokens[0], "exit") == 0) {exit(1);}
    else {
        // Default: execute the command using execvp with all arguments
        execute_command(tokens, background);
    }
}

void execute_commands(char *input, int flag) {
    int semicolon_count = 0;
    int ampersand_count = 0;
    int st_count = 0;
    int ampersand_flag = 0;

    if ((strstr(input, "log") == NULL) && flag == 0) {
        add_log(home_dir, input);
    }

    // Step 1: Tokenize by semicolon ';'
    char *semicolon_token = strtok(input, ";");
    while (semicolon_token != NULL) {
        semicolon_tokens[semicolon_count++] = strdup(semicolon_token);
        semicolon_token = strtok(NULL, ";");
    }

    // Step 2: Process each semicolon-separated command
    for (int i = 0; i < semicolon_count; i++) {
        char *cmd = semicolon_tokens[i];
        cmd = trim_whitespace(cmd);
        // Check for pipes and I/O redirection first
        if (strchr(cmd, '<') != NULL || strchr(cmd, '>') != NULL) {
            // printf("hi\n");
            handle_io_redirection(cmd);
            continue;
        }
        // printf("Hi\n");

        // Step 2a: Iterate backward to check for '&'
        ampersand_flag = 0;  // Reset flag for each command
        int cmd_length = strlen(cmd);
        for (int k = cmd_length - 1; k >= 0; k--) {
            if (isspace((unsigned char)cmd[k])) {
                continue;  // Skip whitespace
            } else if (cmd[k] == '&') {
                ampersand_flag = 1;  // Found '&' at the end
                cmd[k] = '\0';  // Remove '&' from command
                break;
            } else {
                break;  // Non-whitespace, non-'&' character found
            }
        }
        cmd = trim_whitespace(cmd);  // Trim again after potential removal of '&'

        // Step 2b: Tokenize by ampersand '&'
        char *ampersand_token = strtok(cmd, "&");
        ampersand_count = 0;
        while (ampersand_token != NULL) {
            ampersand_tokens[ampersand_count++] = strdup(ampersand_token);
            ampersand_token = strtok(NULL, "&");
        }

        // Step 3: Process each ampersand-separated command
        for (int j = 0; j < ampersand_count; j++) {
            char *sub_cmd = trim_whitespace(ampersand_tokens[j]);

            // Step 4: Tokenize by space/tab " \t"
            char *st_token = strtok(sub_cmd, " \t");
            st_count = 0;
            while (st_token != NULL) {
                space_tab_tokens[st_count++] = strdup(st_token);
                st_token = strtok(NULL, " \t");
            }
            space_tab_tokens[st_count] = NULL;  // Null-terminate

            // Determine if the command should run in the background
            int background = (i == semicolon_count - 1 && j < ampersand_count - 1) ||
                             (i == semicolon_count - 1 && j == ampersand_count - 1 && ampersand_flag == 1);

            // Check and execute the special command or run normally
            check_and_execute_special_command(space_tab_tokens, st_count, background);
        }
    }
}
