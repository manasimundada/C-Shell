#include "reveal.h"

void reveal(const char *path, int show_details, int show_all) {
    DIR *dir;
    struct dirent *entry;
    struct stat mystat;
    char full_path[512];

    // Open the directory
    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // Read and print directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;  // Skip hidden files unless -a is specified
        }

        sprintf(full_path, "%s/%s", path, entry->d_name);
        if (stat(full_path, &mystat) == -1) {
            perror("stat");
            continue;
        }

        if (show_details) {
            reveal_l(entry->d_name, &mystat);
        } else {
            // Determine the color based on file type
            char *color = "\033[33m"; // Default yellow for regular files
            if (S_ISDIR(mystat.st_mode)) {
                color = "\033[34m"; // Blue for directories
            } else if (mystat.st_mode & S_IXUSR) {
                color = "\033[35m"; // Magenta for executables
            }

            // Print file name with color coding
            printf("%s%s\033[0m\n", color, entry->d_name);  // Reset color after printing
        }
    }

    // Close the directory
    closedir(dir);
}

void reveal_l(const char *path, const struct stat *mystat) {
    char *color = "\033[33m";
    if (S_ISDIR(mystat->st_mode)) {
        color = "\033[34m";
    } else if (mystat->st_mode & S_IXUSR) {
        color = "\033[35m";
    }

    printf((S_ISDIR(mystat->st_mode)) ? "d" : "-");
    printf((mystat->st_mode & S_IRUSR) ? "r" : "-");
    printf((mystat->st_mode & S_IWUSR) ? "w" : "-");
    printf((mystat->st_mode & S_IXUSR) ? "x" : "-");
    printf((mystat->st_mode & S_IRGRP) ? "r" : "-");
    printf((mystat->st_mode & S_IWGRP) ? "w" : "-");
    printf((mystat->st_mode & S_IXGRP) ? "x" : "-");
    printf((mystat->st_mode & S_IROTH) ? "r" : "-");
    printf((mystat->st_mode & S_IWOTH) ? "w" : "-");
    printf((mystat->st_mode & S_IXOTH) ? "x" : "-");

    // Print number of hard links
    printf(" %lu", (unsigned long)mystat->st_nlink);

    // Print owner name
    struct passwd *pw = getpwuid(mystat->st_uid);
    printf(" %s", pw ? pw->pw_name : "UNKNOWN");

    // Print group name
    struct group *gr = getgrgid(mystat->st_gid);
    printf(" %s", gr ? gr->gr_name : "UNKNOWN");

    // Print file size
    printf(" %5lld", (long long)mystat->st_size);

    // Print last modification time
    char timebuf[80];
    struct tm *tm = localtime(&mystat->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
    printf(" %s", timebuf);

    printf(" %s%s\033[0m\n", color, path);
}