#include "activities.h"

// Function to compare two Process structs for sorting
int compare_processes(const void *a, const void *b) {
    return strcmp(((struct Process *)a)->command,
                  ((struct Process *)b)->command);
}

void activities_command() {
    // Sort the processes array
    qsort(processes, process_count, sizeof(struct Process), compare_processes);

    // Update the status of each process
    for (int i = 0; i < process_count; i++) {
        if (processes[i].running) {
            int status;  // Declare status here
            pid_t result = waitpid(processes[i].pid, &status, WNOHANG);
            if (result == 0) {
                // Process is still running
                processes[i].running = 1;
            } else if (result > 0) {
                // Process has changed state
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    // Process has terminated
                    processes[i].running = 0;
                } else if (WIFSTOPPED(status)) {
                    // Process is stopped
                    processes[i].running = 1; // Mark as "running" but with "Stopped" status
                } else if (WIFCONTINUED(status)) {
                    // Process was resumed
                    processes[i].running = 1;
                }
            }
        }
    }

    // Print the list of processes, only running or stopped
    for (int i = 0; i < process_count; i++) {
        if (processes[i].running) {  // Only print if the process is still running or stopped
            int status;
            pid_t result = waitpid(processes[i].pid, &status, WNOHANG);
            if (result > 0 && WIFSTOPPED(status)) {
                printf("%d : %s - Stopped\n", processes[i].pid, processes[i].command);
            } else {
                printf("%d : %s - Running\n", processes[i].pid, processes[i].command);
            }
        }
    }
}


// #include "activities.h"
// // Global array to store background process PIDs
// pid_t background_pids[MAX_BG_PROCESSES];
// int bg_process_count = 0;
// // Function to add a background process PID
// void add_background_process(pid_t pid) {
//     if (bg_process_count < MAX_BG_PROCESSES) {
//         background_pids[bg_process_count++] = pid;
//     }
// }
// // Function to remove a background process PID
// void remove_background_process(pid_t pid) {
//     for (int i = 0; i < bg_process_count; i++) {
//         if (background_pids[i] == pid) {
//             for (int j = i; j < bg_process_count - 1; j++) {
//                 background_pids[j] = background_pids[j + 1];
//             }
//             bg_process_count--;
//             break;
//         }
//     }
// }
// // Structure to hold process information
// typedef struct {
//     pid_t pid;
//     char command[256];
//     char state[20];
// } ProcessInfo;
// // Function to compare ProcessInfo structures for sorting
// static int compare_process_info(const void *a, const void *b) {
//     return strcmp(((ProcessInfo*)a)->command, ((ProcessInfo*)b)->command);
// }
// // Function to implement the "activities" command
// void activities_command() {
//     ProcessInfo processes[MAX_BG_PROCESSES];
//     int process_count = 0;
//     // Iterate through background processes
//     for (int i = 0; i < bg_process_count; i++) {
//         pid_t pid = background_pids[i];
//         char stat_path[256];
//         snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
//         FILE *stat_file = fopen(stat_path, "r");
//         if (stat_file) {
//             ProcessInfo info;
//             info.pid = pid;
//             // Read process state and name
//             char state;
//             fscanf(stat_file, "%*d (%[^)]) %c", info.command, &state);
//             // Determine process state
//             strcpy(info.state, (state == 'T') ? "Stopped" : "Running");
//             processes[process_count++] = info;
//             fclose(stat_file);
//         }
//     }
//     // Sort processes lexicographically by command name
//     qsort(processes, process_count, sizeof(ProcessInfo), compare_process_info);
//     // Print sorted process list
//     for (int i = 0; i < process_count; i++) {
//         printf("%d : %s - %s\n", processes[i].pid, processes[i].command, processes[i].state);
//     }
// }
// // Function to handle completed background processes
// void handle_background_processes_a() {
//     int status;
//     pid_t pid;
//     while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
//         if (WIFEXITED(status) || WIFSIGNALED(status)) {
//             printf("\n[%d] %s\n", pid, WIFEXITED(status) ? "Done" : "Terminated");
//             remove_background_process(pid);
//         }
//     }
// }
