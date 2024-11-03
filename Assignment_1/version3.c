#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>

#define INPUT_SIZE 1024

// Function to display the shell prompt with the current directory
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
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Function to execute commands with support for redirection, piping, and background execution
void execute_command(char *input) {
    char *args[20];
    int i = 0;
    int in_redirect = -1, out_redirect = -1, background = 0;
    char *input_file = NULL, *output_file = NULL;

    // Tokenize the input
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            args[i] = NULL;
            input_file = strtok(NULL, " \n");
        } else if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            output_file = strtok(NULL, " \n");
        } else if (strcmp(args[i], "&") == 0) {
            args[i] = NULL;
            background = 1;
            break;
        }
        args[++i] = strtok(NULL, " \n");
    }

    // Check if command contains a pipe
    int pipe_present = 0;
    char *command1[20], *command2[20];
    int pipefd[2];

    for (int j = 0; args[j] != NULL; j++) {
        if (strcmp(args[j], "|") == 0) {
            args[j] = NULL;
            pipe_present = 1;

            for (int k = 0; k < j; k++) command1[k] = args[k];
            command1[j] = NULL;

            int l = 0;
            for (int k = j + 1; args[k] != NULL; k++) command2[l++] = args[k];
            command2[l] = NULL;

            if (pipe(pipefd) == -1) {
                perror("Pipe failed");
                return;
            }
            break;
        }
    }

    if (pipe_present) {
        // Handle commands with pipe
        pid_t pid1 = fork();
        if (pid1 == 0) {
            if (input_file) {
                in_redirect = open(input_file, O_RDONLY);
                dup2(in_redirect, STDIN_FILENO);
                close(in_redirect);
            }
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
            close(pipefd[0]);
            close(pipefd[1]);
            execvp(command1[0], command1);
            perror("Execution failed for first command");
            exit(1);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            if (output_file) {
                out_redirect = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(out_redirect, STDOUT_FILENO);
                close(out_redirect);
            }
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(command2[0], command2);
            perror("Execution failed for second command");
            exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

    } else { // No pipe
        pid_t pid = fork();
        if (pid == 0) {
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
        } else {
            if (!background) {
                waitpid(pid, NULL, 0); // Foreground execution
            } else {
                printf("Process [%d] running in background\n", pid); // Background execution
            }
        }
    }
}

int main() {
    signal(SIGCHLD, handle_sigchld);  // Handle background processes

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
