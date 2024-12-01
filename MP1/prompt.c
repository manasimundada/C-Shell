#include "prompt.h"

char* get_user() {
    struct passwd *p = getpwuid(getuid());
    if (p != NULL) {
        return p->pw_name;
    }
    else {
        perror("getpwuid");
        return NULL;
    }
}

char* get_host() {
    static char hostname[1024];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return hostname;
    } else {
        perror("gethostname");
        return NULL;
    }
}

void print_prompt(char *home_dir) {
    char curr_dir[1024];
    char *user = get_user();
    char *host = get_host();

    if (user == NULL || host == NULL) {
        printf("Error: unable to retrieve user or hostname.\n");
        return;
    }

    printf("<%s@%s:", user, host);

    if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
        if (strncmp(curr_dir, home_dir, strlen(home_dir)) == 0) {
            const char *rel_path = curr_dir + strlen(home_dir);
            printf("~%s>", rel_path);
        } else {
            printf("%s>", curr_dir);
        }
    } else {
        perror("getcwd");
    }
}

