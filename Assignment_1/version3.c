#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>

#define INPUT_SIZE 1024

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd error");
    }
}

// Signal handler to clean up zombie processes
void handle_sigchld(int sig) {
    // Reap all terminated child processes
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Execute the command, with support for background processing
void execute_command(char *input) {
    char *args[20];
    int i = 0;
    int background = 0;

    // Parse the command and check for '&' at the end
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \n");
    }

    // Check if the last argument is '&' for background processing
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        background = 1;      // Set the background flag
        args[i - 1] = NULL;  // Remove '&' from arguments
    }

    // Fork the process to execute the command
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) { // Child process
        execvp(args[0], args);
        perror("Execution failed"); // If execvp fails
        exit(1);
    } else { // Parent process
        if (!background) {
            // Wait for the child process if it's a foreground command
            waitpid(pid, NULL, 0);
        } else {
            // Immediately return prompt for background command
            printf("[1] %d\n", pid); // Output format [1] PID
        }
    }
}

int main() {
    // Set up signal handler for SIGCHLD to clean up zombie processes
    signal(SIGCHLD, handle_sigchld);

    char input[INPUT_SIZE];

    while (1) {
        display_prompt();

        // Read user input
        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            printf("\nExiting shell...\n");
            break;
        }

        if (input[0] != '\n') {
            execute_command(input);
        }
    }

    return 0;
}
