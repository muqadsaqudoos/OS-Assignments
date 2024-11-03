#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <signal.h>

#define MAX_ARGS 100
#define MAX_JOBS 10
#define MAX_VAR_LEN 50
#define MAX_CMD_LEN 1024

// Struct for background job
typedef struct {
    int pid;
    char command[MAX_CMD_LEN];
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;

// Struct for user-defined variable
typedef struct {
    char name[MAX_VAR_LEN];
    char value[MAX_VAR_LEN];
} Variable;

Variable variables[MAX_ARGS];
int var_count = 0;

void display_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PUCITshell@%s:- ", cwd);
    } else {
        perror("getcwd error");
    }
}

void add_job(int pid, char* command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, MAX_CMD_LEN);
        job_count++;
    }
}

void remove_job(int pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            job_count--;
            break;
        }
    }
}

void list_jobs() {
    printf("Jobs:\n");
    for (int i = 0; i < job_count; i++) {
        printf("[%d] PID: %d Command: %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}

void kill_job(int job_num) {
    if (job_num > 0 && job_num <= job_count) {
        int pid = jobs[job_num - 1].pid;
        if (kill(pid, SIGKILL) == 0) {
            printf("Killed job [%d] with PID %d\n", job_num, pid);
            remove_job(pid);
        } else {
            perror("Failed to kill job");
        }
    } else {
        printf("Invalid job number.\n");
    }
}

void execute_command(char* input);

// Handling history command execution
void execute_history_command(int index) {
    if (index < 1 || index > history_length) {
        printf("No command found at index %d\n", index);
        return;
    }
    HIST_ENTRY **the_history = history_list();
    char* command_to_execute = the_history[index - 1]->line;
    printf("Executing from history: %s\n", command_to_execute);
    execute_command(command_to_execute);
}

// Function to set user-defined variable
void set_variable(char* name, char* value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            strncpy(variables[i].value, value, MAX_VAR_LEN);
            return;
        }
    }
    if (var_count < MAX_ARGS) {
        strncpy(variables[var_count].name, name, MAX_VAR_LEN);
        strncpy(variables[var_count].value, value, MAX_VAR_LEN);
        var_count++;
    }
}

// Function to get variable value
char* get_variable(char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    return NULL;
}

// Function to list variables
void list_variables() {
    printf("Variables:\n");
    for (int i = 0; i < var_count; i++) {
        printf("%s=%s\n", variables[i].name, variables[i].value);
    }
}

void execute_command(char* input) {
    char* args[MAX_ARGS];
    int background = 0;

    // Check for & to set background execution
    if (input[strlen(input) - 1] == '&') {
        background = 1;
        input[strlen(input) - 1] = '\0';
    }

    // Tokenize input
    int arg_count = 0;
    args[arg_count] = strtok(input, " \n");
    while (args[arg_count] != NULL && arg_count < MAX_ARGS - 1) {
        args[++arg_count] = strtok(NULL, " \n");
    }
    args[arg_count] = NULL;

    // Built-in commands
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || chdir(args[1]) != 0) {
            perror("cd error");
        }
        return;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(args[0], "jobs") == 0) {
        list_jobs();
        return;
    }
    if (strcmp(args[0], "kill") == 0) {
        if (args[1]) {
            int job_num = atoi(args[1]);
            kill_job(job_num);
        } else {
            printf("Usage: kill <job number>\n");
        }
        return;
    }
    if (strcmp(args[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("cd <dir>, exit, jobs, kill <job number>, help\n");
        return;
    }
    if (strcmp(args[0], "history") == 0) {
        HIST_ENTRY **the_history = history_list();
        for (int i = 0; i < history_length; i++) {
            printf("%d: %s\n", i + 1, the_history[i]->line);
        }
        return;
    }
    if (args[0][0] == '!') {
        int hist_index = (args[0][1] == '-') ? history_length - atoi(&args[0][2]) : atoi(&args[0][1]);
        execute_history_command(hist_index);
        return;
    }
    if (strchr(args[0], '=')) {  // User-defined variable
        char* name = strtok(args[0], "=");
        char* value = strtok(NULL, "=");
        set_variable(name, value);
        return;
    }
    if (strcmp(args[0], "list_variables") == 0) {
        list_variables();
        return;
    }

    // Process external commands with piping and redirection
    int pipefd[2];
    int pipe_present = 0;
    char* command1[MAX_ARGS];
    char* command2[MAX_ARGS];
    int cmd1_count = 0, cmd2_count = 0;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            pipe_present = 1;
            args[i] = NULL;
            for (int j = 0; j < i; j++) command1[cmd1_count++] = args[j];
            command1[cmd1_count] = NULL;
            for (int j = i + 1; args[j] != NULL; j++) command2[cmd2_count++] = args[j];
            command2[cmd2_count] = NULL;
            break;
        }
    }

    if (pipe_present) {
        pipe(pipefd);
        if (fork() == 0) {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            execvp(command1[0], command1);
            perror("Execution failed");
            exit(1);
        }
        if (fork() == 0) {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(command2[0], command2);
            perror("Execution failed");
            exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        wait(NULL);
        wait(NULL);
    } else {
        if (fork() == 0) {
            if (background) {
                setpgid(0, 0);
            }
            execvp(args[0], args);
            perror("Execution failed");
            exit(1);
        }
        if (!background) {
            wait(NULL);
        } else {
            printf("Process running in background with PID %d\n", getpid());
            add_job(getpid(), input);
        }
    }
}

int main() {
    char* input;
    using_history();
    while (1) {
        display_prompt();
        input = readline("");
        if (strlen(input) > 0) {
            add_history(input);
            execute_command(input);
        }
        free(input);
    }
    return 0;
}
