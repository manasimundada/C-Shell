#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>

// Function to get the most recently created process ID
pid_t get_recent_process_pid() {
    DIR *dir;
    struct dirent *entry;
    pid_t recent_pid = -1;
    pid_t current_pid;

    // Open the /proc directory
    if ((dir = opendir("/proc")) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Iterate through the directories in /proc
    while ((entry = readdir(dir)) != NULL) {
        // Check if the directory name is a number (which indicates a process)
        if (entry->d_type == DT_DIR && (current_pid = atoi(entry->d_name)) > 0) {
            // Keep track of the largest PID found
            if (current_pid > recent_pid) {
                recent_pid = current_pid;
            }
        }
    }

    closedir(dir);
    return recent_pid;
}

// Function to check if a key was pressed (non-blocking) using select()
int kbhit() {
    struct termios oldt, newt;
    struct timeval tv;
    fd_set readfds;

    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    // Set timeout to 0 (non-blocking)
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    // Check if there's any input on stdin
    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return result > 0;  // Returns 1 if input is available, 0 otherwise
}

// Function to run the neonate command
void neonate_command(int time_arg) {
    char ch;
    while (1) {
        // Get the most recently created process ID
        pid_t recent_pid = get_recent_process_pid();
        if (recent_pid != -1) {
            printf("Most recently created process ID: %d\n", recent_pid);
        } else {
            printf("No process found.\n");
        }

        // Sleep for the specified time_arg
        sleep(time_arg);

        // Check if 'x' is pressed using kbhit and get the character only if it's pressed
        if (kbhit()) {
            ch = getchar();  // Only call getchar() if kbhit() detects a key press
            if (ch == 'x') {
                printf("Exiting...\n");
                break;
            }
        }
    }
}