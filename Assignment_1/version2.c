#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>

#define INPUT_SIZE 1024

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd error");
    }
}

void execute_single_command(char *command) {
    char *args[20];
    int i = 0;
    args[i] = strtok(command, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }
    execvp(args[0], args);
    perror("Execution failed"); // If execvp fails
    exit(1);
}

void execute_command(char *input) {
    char *args[20];
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }

    int in_redirect = -1, out_redirect = -1;
    char *input_file = NULL;
    char *output_file = NULL;

    // Check for redirection symbols
    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "<") == 0) {
            input_file = args[j + 1];
            args[j] = NULL;
        } else if (strcmp(args[j], ">") == 0) {
            output_file = args[j + 1];
            args[j] = NULL;
        }
    }

    int pipefd[2];
    int pipe_present = 0;

    // Check for pipes
    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "|") == 0) {
            args[j] = NULL;
            pipe_present = 1;
            if (pipe(pipefd) == -1) {
                perror("Pipe failed");
                return;
            }
            break;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) { // Child process
        if (input_file) {
            in_redirect = open(input_file, O_RDONLY);
            if (in_redirect < 0) {
                perror("Failed to open input file");
                exit(1);
            }
            dup2(in_redirect, STDIN_FILENO);
            close(in_redirect);
        }

        if (output_file) {
            out_redirect = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_redirect < 0) {
                perror("Failed to open output file");
                exit(1);
            }
            dup2(out_redirect, STDOUT_FILENO);
            close(out_redirect);
        }

        if (pipe_present) {
            if (fork() == 0) { // First part of pipe command
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                execvp(args[0], args);
                perror("Execution failed");
                exit(1);
            }
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(args[i + 1], args + i + 1);
            perror("Execution failed");
            exit(1);
        } else {
            execvp(args[0], args);
            perror("Execution failed");
            exit(1);
        }
    } else { // Parent process
        wait(NULL); // Wait for child process to complete
    }
}

int main() {
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
