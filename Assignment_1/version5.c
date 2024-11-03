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

typedef struct BackgroundProcess {
    int pid;
    char command[INPUT_SIZE];
} BackgroundProcess;

BackgroundProcess bg_processes[HISTORY_LIMIT];
int bg_count = 0;

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

void handle_sigchld(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Remove process from background list when it completes
        for (int i = 0; i < bg_count; i++) {
            if (bg_processes[i].pid == pid) {
                printf("\n[Completed] Background process [%d] %s\n", pid, bg_processes[i].command);
                for (int j = i; j < bg_count - 1; j++) {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_count--;
                break;
            }
        }
    }
}

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
    add_history(command);
}

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

void save_history() {
    FILE *file = fopen(HISTORY_FILE, "w");
    if (file == NULL) return;
    for (int i = 0; i < history_count; i++) {
        fprintf(file, "%s\n", custom_history[i]);
    }
    fclose(file);
}

void add_background_process(int pid, char *command) {
    if (bg_count < HISTORY_LIMIT) {
        bg_processes[bg_count].pid = pid;
        strncpy(bg_processes[bg_count].command, command, INPUT_SIZE);
        bg_count++;
    }
}

void list_jobs() {
    printf("Background jobs:\n");
    for (int i = 0; i < bg_count; i++) {
        printf("[%d] %d %s\n", i + 1, bg_processes[i].pid, bg_processes[i].command);
    }
}

void kill_job(int job_number) {
    if (job_number < 1 || job_number > bg_count) {
        printf("Invalid job number.\n");
        return;
    }
    int pid = bg_processes[job_number - 1].pid;
    if (kill(pid, SIGKILL) == 0) {
        printf("Process %d killed successfully.\n", pid);
        for (int i = job_number - 1; i < bg_count - 1; i++) {
            bg_processes[i] = bg_processes[i + 1];
        }
        bg_count--;
    } else {
        perror("Failed to kill process");
    }
}

void execute_builtin_command(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd error");
        }
    } else if (strcmp(args[0], "exit") == 0) {
        save_history();
        exit(0);
    } else if (strcmp(args[0], "jobs") == 0) {
        list_jobs();
    } else if (strcmp(args[0], "kill") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "kill: expected job number\n");
        } else {
            kill_job(atoi(args[1]));
        }
    } else if (strcmp(args[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("cd <dir> - Change directory\n");
        printf("exit - Exit the shell\n");
        printf("jobs - List background jobs\n");
        printf("kill <job_number> - Kill background job\n");
        printf("help - List built-in commands\n");
    }
}

int is_builtin_command(char *cmd) {
    return strcmp(cmd, "cd") == 0 || strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "jobs") == 0 || strcmp(cmd, "kill") == 0 ||
           strcmp(cmd, "help") == 0;
}

void execute_command(char *input) {
    if (input[0] == '!') {
        int history_index = (strcmp(input, "!-1") == 0) ? history_count - 1 : atoi(input + 1) - 1;
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
    int i = 0, in_redirect = -1, out_redirect = -1, background = 0;
    char *input_file = NULL, *output_file = NULL;

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

    if (args[0] == NULL) return;
    if (is_builtin_command(args[0])) {
        execute_builtin_command(args);
        return;
    }

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
        pid_t pid1 = fork();
        if (pid1 == 0) {
            if (input_file) {
                in_redirect = open(input_file, O_RDONLY);
                dup2(in_redirect, STDIN_FILENO);
                close(in_redirect);
            }
            dup2(pipefd[1], STDOUT_FILENO);
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
            dup2(pipefd[0], STDIN_FILENO);
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

    } else {
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
                waitpid(pid, NULL, 0);
            } else {
                printf("Process [%d] running in background\n", pid);
                add_background_process(pid, input);
            }
        }
    }

    if (input[0] == '!') free(input);
}

int main() {
    signal(SIGCHLD, handle_sigchld);

    load_history();
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

    save_history();
    for (int i = 0; i < history_count; i++) {
        free(custom_history[i]);
    }

    return 0;
}
