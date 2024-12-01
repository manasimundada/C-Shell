#include "proclore.h"

void print_proclore(pid_t pid){
    char path[256];
    char buffer[1024];
    char exe_path[1024];
    char status;
    long vm_size;

    if (kill(pid, 0) == -1) {
        if (errno == ESRCH) {
            printf("Process with PID %d does not exist.\n", pid);
        } else {
            perror("kill");
        }
        return;
    }

    printf("pid: %d\n", pid);
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }
    fscanf(fp, "%*d %*s %c", &status); // The third field in /proc/[pid]/stat is the process status
    fclose(fp);
    printf("process status: %c\n", status);
    
    pid_t pgid = getpgid(pid);
    if (pgid == -1) {
        perror("getpgid");
        return;
    }
    printf("process group: %d\n", pgid);
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }
    fscanf(fp, "%ld", &vm_size);
    printf("virtual memory: %ld KB\n", vm_size * 4);
    fclose(fp);    

    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink");
        return;
    }
    exe_path[len] = '\0';
    printf("executable path: %s\n", exe_path);
}