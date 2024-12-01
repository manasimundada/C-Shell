#include "seek.h"

#define MAX_PATH 1024

void print_colored(const char* path, bool is_dir) {
    if (is_dir) {
        printf("\033[1;34m%s\033[0m\n", path);  // Blue for directories
    } else {
        printf("\033[1;32m%s\033[0m\n", path);  // Green for files
    }
}

char* resolve_path(const char* path) {
    char* resolved_path = realpath(path, NULL);
    if (resolved_path == NULL) {
        perror("Error resolving path");
        exit(1);
    }
    return resolved_path;
}

void search_directory(const char* base_path, const char* dir_path, const char* target, bool only_dirs, bool only_files, bool exec_flag, int* match_count) {
    DIR* dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent* entry;
    char matches[MAX_PATH][MAX_PATH];  // Store matching paths
    int matches_index = 0;  // Track the number of matches
    static bool content_printed = false;  // Track if file contents have been printed

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (lstat(full_path, &st) == -1) {
            perror("Error getting file status");
            continue;
        }

        bool is_dir = S_ISDIR(st.st_mode);

        char name_to_file[1024];
        strcpy(name_to_file, target);
        strncat(name_to_file, ".", 1);
        if (strcmp(entry->d_name, target) == 0 || strncmp(entry->d_name, name_to_file, strlen(name_to_file)) == 0) {
            if ((is_dir && !only_files) || (!is_dir && !only_dirs)) {
                char relative_path[MAX_PATH];
                snprintf(relative_path, sizeof(relative_path), "./%s", full_path + strlen(base_path) + 1);
                strcpy(matches[matches_index++], relative_path);  // Store the match
                (*match_count)++;
            }
        }

        if (is_dir) {
            search_directory(base_path, full_path, target, only_dirs, only_files, exec_flag, match_count);
        }
    }

    closedir(dir);

    // Handle cases based on exec_flag and match conditions
    // Modify this block in the search_directory function
    if (exec_flag == 0) {
        for (int i = 0; i < matches_index; i++) {
            // Check if the match is a directory or file before printing
            struct stat st;
            if (lstat(matches[i] + 2, &st) == -1) {  // Skip "./" for actual path
                perror("Error getting file status");
                continue;
            }
            print_colored(matches[i], S_ISDIR(st.st_mode));  // Correctly determine if it's a directory or file
        }
    } else if (exec_flag == 1) {
        if (*match_count > 0 && only_dirs) {
            for (int i = 0; i < matches_index; i++) {
                print_colored(matches[i], true);  // All are directories, so is_dir is true
            }
        } else if (*match_count == 1 && only_dirs) {
            print_colored(matches[0], true);
            chdir(matches[0]);  // Change to the directory
        } else if (*match_count > 1 && only_files) {
            for (int i = 0; i < matches_index; i++) {
                print_colored(matches[i], false);  // All are files, so is_dir is false
            }
        } else if (*match_count == 1 && only_files && !content_printed) {
            // Print the contents of the file
            FILE* file = fopen(matches[0] + 2, "r");  // Skip "./" to get the actual path
            if (file == NULL) {
                printf("Missing permissions for task!\n");
                return;
            }
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                printf("%s", buffer);
            }
            fclose(file);

            content_printed = true;  // Mark that the content has been printed
        }
    }

}
