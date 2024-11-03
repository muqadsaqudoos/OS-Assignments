#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>

#define INPUT_SIZE 1024

// Function to display the shell prompt
void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd error");
    }
}

// Function to execute commands with optional redirection and piping
void execute_command(char *input) {
    char *args[20];
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }

    if (args[0] == NULL) {
        return; // No command entered
    }

    int in_redirect = -1, out_redirect = -1;
    char *input_file = NULL;
    char *output_file = NULL;

    // Check for redirection symbols and handle errors
    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "<") == 0) {
            if (args[j + 1] == NULL) {
                fprintf(stderr, "Expected input file after '<'\n");
                return;
            }
            input_file = args[j + 1];
            args[j] = NULL;
        } else if (strcmp(args[j], ">") == 0) {
            if (args[j + 1] == NULL) {
                fprintf(stderr, "Expected output file after '>'\n");
                return;
            }
            output_file = args[j + 1];
            args[j] = NULL;
        }
    }

    int pipefd[2];
    int pipe_present = 0;
    char *command1[20], *command2[20];

    // Check for pipes
    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "|") == 0) {
            args[j] = NULL;
            pipe_present = 1;

            // Split command into two parts for piping
            for (int k = 0; k < j; k++) {
                command1[k] = args[k];
            }
            command1[j] = NULL;

            int l = 0;
            for (int k = j + 1; args[k] != NULL; k++) {
                command2[l++] = args[k];
            }
            command2[l] = NULL;

            if (pipe(pipefd) == -1) {
                perror("Pipe failed");
                return;
            }
            break;
        }
    }

    if (pipe_present) {
        // Handle pipe operations
        pid_t pid1 = fork();
        if (pid1 < 0) {
            perror("Fork failed for first command");
            return;
        } else if (pid1 == 0) { // First command in the pipe
            if (input_file) {
                in_redirect = open(input_file, O_RDONLY);
                if (in_redirect < 0) {
                    perror("Failed to open input file");
                    exit(1);
                }
                dup2(in_redirect, STDIN_FILENO);
                close(in_redirect);
            }
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe's write end
            close(pipefd[0]); // Close unused read end of pipe
            close(pipefd[1]); // Close the write end after duplication
            execvp(command1[0], command1);
            perror("Execution failed for first command");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("Fork failed for second command");
            return;
        } else if (pid2 == 0) { // Second command in the pipe
            if (output_file) {
                out_redirect = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out_redirect < 0) {
                    perror("Failed to open output file");
                    exit(1);
                }
                dup2(out_redirect, STDOUT_FILENO);
                close(out_redirect);
            }
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe's read end
            close(pipefd[1]); // Close unused write end of pipe
            close(pipefd[0]); // Close the read end after duplication
            execvp(command2[0], command2);
            perror("Execution failed for second command");
            exit(1);
        }

        // Close the pipe in the parent process
        close(pipefd[0]);
        close(pipefd[1]);

        // Wait for both child processes to complete
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

    } else { // No pipe present
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

            execvp(args[0], args);
            perror("Execution failed");
            exit(1);
        } else { // Parent process
            wait(NULL); // Wait for the child process to complete
        }
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
