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

// Custom history array
char *custom_history[HISTORY_LIMIT];
int history_count = 0;

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
        // Free the oldest command and shift history
        free(custom_history[0]);
        for (int i = 1; i < HISTORY_LIMIT; i++) {
            custom_history[i - 1] = custom_history[i];
        }
        custom_history[HISTORY_LIMIT - 1] = strdup(command);
    }

    // Add to readline's history for up/down arrow navigation
    add_history(command);
}

// Load custom history from file
void load_history() {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (file == NULL) return;
    
    char line[INPUT_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character
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

// Execute commands, including !number for history
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
    } else {
        add_to_custom_history(input);
    }

    char *args[20];
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else {
        waitpid(pid, NULL, 0);
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
