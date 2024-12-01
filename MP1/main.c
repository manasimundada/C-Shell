#include "prompt.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "tokenise.h"
#include "seek.h"
#include "tokenise.h"
// #include "pipes.h"
#include "alias.h"

char home_dir[1024];

int main() {
    char input[1024];

    // Initialize the home directory
    if (getcwd(home_dir, sizeof(home_dir)) == NULL) {
        perror("getcwd");
        return 1;
    }
    initialize_signal_handlers();
    // printf("Debug: Signal handlers initialized\n");
    // fflush(stdout);

    while (1) {
        print_prompt(home_dir);  // Assuming this function exists for printing the prompt

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            exit(1);
        }
        input[strcspn(input, "\n")] = 0;  // Remove newline character
        execute_commands(input, 0);  // Call the function from tokenise.c
    }

    return 0;
}
