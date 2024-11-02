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
#define MAX_BG_PROCESSES 10

// Background process structure
typedef struct {
    int pid;
    char command[1024];
} BackgroundProcess;

BackgroundProcess bg_processes[MAX_BG_PROCESSES];
int bg_count = 0;

// Sample commands for completion
const char *sample_commands[] = { "ls", "pwd", "echo", "whoami", "cd", "history", "exit", "jobs", "kill", "help", NULL };

void display_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd error");
    }
}

// Signal handler to clean up zombie processes and remove completed background processes
void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < bg_count; i++) {
            if (bg_processes[i].pid == pid) {
                printf("\n[Done] %d: %s\n", pid, bg_processes[i].command);
                // Shift the remaining processes
                for (int j = i; j < bg_count - 1; j++) {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_count--;
                break;
            }
        }
    }
}

// Add a process to background job list
void add_bg_process(int pid, char *command) {
    if (bg_count < MAX_BG_PROCESSES) {
        bg_processes[bg_count].pid = pid;
        strncpy(bg_processes[bg_count].command, command, sizeof(bg_processes[bg_count].command));
        bg_count++;
    } else {
        printf("Maximum background process limit reached.\n");
    }
}

// Built-in command: `cd`
void shell_cd(char *path) {
    if (path == NULL) {
        fprintf(stderr, "cd: expected argument\n");
    } else if (chdir(path) != 0) {
        perror("cd");
    }
}

// Built-in command: `exit`
void shell_exit() {
    printf("Exiting shell...\n");
    exit(0);
}

// Built-in command: `jobs`
void shell_jobs() {
    printf("Background Jobs:\n");
    for (int i = 0; i < bg_count; i++) {
        printf("[%d] %d: %s\n", i + 1, bg_processes[i].pid, bg_processes[i].command);
    }
}

// Updated `kill` command to use PID directly
// Built-in command: `kill`
void shell_kill(int pid) {
    int found = 0;
    for (int i = 0; i < bg_count; i++) {
        if (bg_processes[i].pid == pid) {
            // Send SIGKILL to the specified PID
            if (kill(pid, SIGKILL) == 0) {
                printf("Killed process %d\n", pid);
                found = 1;
                // Remove the process from the background list
                for (int j = i; j < bg_count - 1; j++) {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_count--;
                break;
            } else {
                perror("kill");
            }
        }
    }
    // Only display "Invalid PID" message if not found in bg_processes list
    if (!found) {
        fprintf(stderr, "Invalid PID: %d\n", pid);
    }
}

// Built-in command: `help`
void shell_help() {
    printf("Available built-in commands:\n");
    printf("  cd <path>     : Change the current directory\n");
    printf("  exit          : Exit the shell\n");
    printf("  jobs          : List background jobs\n");
    printf("  kill <PID>    : Kill a background job by its PID\n");
    printf("  help          : Display this help message\n");
}

// Check if a command is built-in and execute it
int execute_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        shell_cd(args[1]);
        return 1;
    }
    if (strcmp(args[0], "exit") == 0) {
        shell_exit();
        return 1;
    }
    if (strcmp(args[0], "jobs") == 0) {
        shell_jobs();
        return 1;
    }
    if (strcmp(args[0], "kill") == 0) {
        if (args[1]) {
            shell_kill(atoi(args[1]));  // Pass PID directly
        } else {
            printf("Usage: kill <PID>\n");
        }
        return 1;
    }
    if (strcmp(args[0], "help") == 0) {
        shell_help();
        return 1;
    }
    return 0;
}

// Execute external commands
void execute_command(char *input) {
    add_history(input);

    char *args[20];
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL) {
        args[++i] = strtok(NULL, " \n");
    }

    if (args[0] == NULL) return;  // Empty command

    // Check if the command is built-in
    if (execute_builtin(args)) return;

    // Handle background process '&'
    int background = 0;
    if (i > 1 && strcmp(args[i - 1], "&") == 0) {
        background = 1;
        args[i - 1] = NULL;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) { // Child process
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else { // Parent process
        if (background) {
            add_bg_process(pid, input);
            printf("[Background] %d: %s\n", pid, input);
        } else {
            waitpid(pid, NULL, 0);
        }
    }
}

int main() {
    signal(SIGCHLD, handle_sigchld);  // Clean up background processes

    // Load history from file
    read_history(HISTORY_FILE);
    stifle_history(HISTORY_LIMIT);  // Limit history to 10 commands

    char *input;
    while (1) {
        display_prompt();

        input = readline("");
        if (!input) {
            printf("\nExiting shell...\n");
            break;
        }

        if (strlen(input) > 0) {
            execute_command(input);
        }

        free(input);
    }

    write_history(HISTORY_FILE);
    history_truncate_file(HISTORY_FILE, HISTORY_LIMIT);

    return 0;
}
