#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#define HISTORY_FILE ".myshell_history"
#define HISTORY_LIMIT 10
#define INPUT_SIZE 1024

char *custom_history[HISTORY_LIMIT];
int history_count = 0;

// Display the shell prompt with current directory
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

// Add command to custom history with a limit of 10 commands
void add_to_custom_history(char *command) {
    if (history_count < HISTORY_LIMIT) {
        custom_history[history_count] = strdup(command);
        history_count++;
    } else {
        free(custom_history[0]);
        for (int i = 1; i < HISTORY_LIMIT; i++) {
            custom_history[i - 1] = custom_history[i];
        }
        custom_history[HISTORY_LIMIT - 1] = strdup(command);
    }
    add_history(command);  // Add to readline's history for navigation
}

// Load custom history from file
void load_history() {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (file == NULL) return;
    
    char line[INPUT_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        add_to_custom_history(line);
    }
    fclose(file);
}

// Save custom history to file
void save_history() {
    FILE *file = fopen(HISTORY_FILE, "w");
    if (file == NULL) return;

    for (int i = 0; i < history_count; i++) {
        fprintf(file, "%s\n", custom_history[i]);
    }
    fclose(file);
}

// List the last 10 commands from custom history
void list_history() {
    printf("Command History:\n");
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, custom_history[i]);
    }
}

// Execute commands, including handling redirection, piping, background execution, and history
void execute_command(char *input) {
    // Check if input is a history command, like "!number"
    if (input[0] == '!') {
        int history_index;
        if (strcmp(input, "!-1") == 0) {  // Repeat last command
            history_index = history_count - 1;
        } else {
            history_index = atoi(input + 1) - 1;
        }

        if (history_index < 0 || history_index >= history_count) {
            printf("No such command in history.\n");
            return;
        }

        printf("Repeating command: %s\n", custom_history[history_index]);
        input = strdup(custom_history[history_index]);
    } else if (strcmp(input, "history") == 0) {  // Check for the history command
        list_history();
        return;
    } else {
        add_to_custom_history(input);
    }

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
        // Handle pipe operations
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

    } else { // No pipe present
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

    if (input[0] == '!') free(input);
}

int main() {
    signal(SIGCHLD, handle_sigchld);  // Handle background processes

    // Load history from file into custom history array and readline's history
    load_history();

    char *input;
    while (1) {
        display_prompt();
        input = readline("");  // Use readline to support arrow key navigation

        if (!input) {  // Handle Ctrl+D for exit
            printf("\nExiting shell...\n");
            break;
        }

        if (strlen(input) > 0) {
            execute_command(input);
        }

        free(input);
    }

    // Save custom history to file before exiting
    save_history();

    // Free custom history array
    for (int i = 0; i < history_count; i++) {
        free(custom_history[i]);
    }

    return 0;
}
