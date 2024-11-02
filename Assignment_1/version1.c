
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

#define INPUT_SIZE 1024  // Changed from MAX_INPUT to INPUT_SIZE

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd error");
    }
}

void execute_command(char *input) {
    // Parse input
    char *args[20];
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }

    // Fork and execute
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) {
        // Child process
        if (execvp(args[0], args) < 0) {
            perror("Command execution failed");
        }
        exit(0);
    } else {
        // Parent process waits for child
        wait(NULL);
    }
}

int main() {
    char input[INPUT_SIZE];  // Use the renamed INPUT_SIZE here

    while (1) {
        display_prompt();

        // Read user input
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("\nExiting shell...\n");
            break;  // Exit if CTRL+D is pressed (EOF)
        }

        // Remove trailing newline character
        if (input[0] != '\n') {
            execute_command(input);
        }
    }

    return 0;
}
