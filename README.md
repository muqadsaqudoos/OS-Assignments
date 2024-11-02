#OS ASSIGNMENT 1
## My Custom Shell - Version 01

This project is a simple UNIX-like shell created for learning purposes. The shell is built with basic functionalities using system calls in C.

### Features Implemented

- **Custom Prompt**: Displays the current working directory as the prompt, labeled as `PUCITshell`.
- **Command Execution**: Supports executing UNIX commands entered by the user.
- **Exit Condition**: Exits the shell when `CTRL+D` is pressed.

### File Structure

- `shell_v1.c`: This file contains all the code for Version 01 of the shell, including displaying the prompt, parsing input, and executing commands.

### Known Issues and Errors

- **MAX_INPUT Warning**: Initially, there was a conflict due to a predefined `MAX_INPUT` in system headers. This was resolved by renaming it to `INPUT_SIZE`.
- **Error Handling**: Basic error messages are displayed if commands fail or if the `fork()` system call does not work as expected.

### Limitations

- **No Redirection Support**: Version 01 does not support input/output redirection (e.g., using `<` or `>` symbols).
- **No Piping Functionality**: This version does not handle pipes (e.g., commands with `|`), so commands cannot be chained.
- **Basic Error Handling**: Limited error handling is available, which may not cover all edge cases in command execution failures.

### Acknowledgments

This project was developed with assistance from the following resources:
- **Arif Butt**: My instructor, who provided valuable guidance on UNIX system programming concepts. GitHub Profile: [Arif Butt](https://github.com/arifpucit)
- UNIX system programming guides and online documentation for system calls.


## My Custom Shell - Version 02

This project is a simple UNIX-like shell created for learning purposes. The shell is built with extended functionalities using system calls in C, including support for input/output redirection and piping.

### Features Implemented

- **Custom Prompt**: Displays the current working directory as the prompt, labeled as `PUCITshell`.
- **Basic Command Execution**: Supports executing standard UNIX commands entered by the user, such as `ls`, `pwd`, and `echo`.
- **Input and Output Redirection**:
  - Supports `<` for redirecting input from a file and `>` for redirecting output to a file.
  - Example: `cat < input.txt > output.txt` reads from `input.txt` and writes to `output.txt`.
- **Piping**:
  - Allows chaining commands using `|` so that the output of one command becomes the input of another.
  - Example: `ls | wc -l` lists files in the directory and counts them.
- **Exit Condition**: Exits the shell when `CTRL+D` is pressed.

### File Structure

- `shell_v2.c`: This file contains all the code for Version 02 of the shell, including displaying the prompt, parsing input, handling redirection, and implementing piping.

### Known Issues and Limitations

- **Single Pipe Limitation**: Currently, the shell supports only a single `|` in commands. Chaining multiple pipes (e.g., `ls | grep txt | wc -l`) is not yet implemented.
- **Error Handling**: Basic error handling is in place for command execution failures and file opening errors, but there may still be scenarios where error messages are limited or less informative.
- **Fixed Redirection and Pipe Handling Issue**: An earlier issue caused the shell to hang when using pipe commands like `cat /etc/passwd | wc`. This was resolved by ensuring that all unused file descriptors are closed properly in both child and parent processes.

### Acknowledgments

This project was developed with assistance from the following resources:
- **Arif Butt**: My instructor, who provided valuable guidance on UNIX system programming concepts. GitHub Profile: [Arif Butt](https://github.com/arifpucit)
- **ChatGPT by OpenAI**: Assisted in implementing and troubleshooting various aspects of the shell, specifically for managing `execvp`, `fork`, `wait`, `dup2`, and pipe handling in C.
- UNIX system programming guides and online documentation for system calls.

## My Custom Shell - Version 03

This project is a simple UNIX-like shell created for learning purposes. The shell is built with advanced functionalities using system calls in C, including support for input/output redirection, piping, and background command execution.

### Features Implemented

- **Custom Prompt**: Displays the current working directory as the prompt, labeled as `PUCITshell`.
- **Basic Command Execution**: Supports executing standard UNIX commands entered by the user, such as `ls`, `pwd`, and `echo`.
- **Input and Output Redirection**:
  - Supports `<` for redirecting input from a file and `>` for redirecting output to a file.
  - Example: `cat < input.txt > output.txt` reads from `input.txt` and writes to `output.txt`.
- **Piping**:
  - Allows chaining commands using `|` so that the output of one command becomes the input of another.
  - Example: `ls | wc -l` lists files in the directory and counts them.
- **Background Command Execution**:
  - Supports running commands in the background by appending `&` at the end of the command.
  - Example: `find / -name f1.txt &` starts the `find` command in the background, allowing the user to continue using the shell immediately.
  - Background process IDs are displayed in the format `[1] PID`.
- **Exit Condition**: Exits the shell when `CTRL+D` is pressed.

### File Structure

- `shell_v3.c`: This file contains all the code for Version 03 of the shell, including displaying the prompt, parsing input, handling redirection, implementing piping, and supporting background command execution.

### Known Issues and Limitations

- **Single Pipe Limitation**: Currently, the shell supports only a single `|` in commands. Chaining multiple pipes (e.g., `ls | grep txt | wc -l`) is not yet implemented.
- **Limited Background Job Management**: Background processes are supported, but there is no comprehensive job management system. Only a basic `[1] PID` display is provided, and job control commands like `fg` and `bg` are not implemented.
- **Error Handling**: Basic error handling is in place for command execution failures, file opening errors, and redirection, but there may still be scenarios where error messages are limited or less informative.

### Acknowledgments

This project was developed with assistance from the following resources:
- **Arif Butt**: My instructor, who provided valuable guidance on UNIX system programming concepts. GitHub Profile: [Arif Butt](https://github.com/arifpucit)
- **ChatGPT by OpenAI**: Assisted in implementing and troubleshooting various aspects of the shell, specifically for managing `execvp`, `fork`, `wait`, `dup2`, signal handling, and pipe handling in C.
- UNIX system programming guides and online documentation for system calls.

## My Custom Shell - Version 04

This project is a UNIX-like shell created for educational purposes, with features such as limited command history, command repetition using custom syntax, and navigation through the history using up/down arrow keys. The shell is developed in C, using system calls and the `readline` library.

### Features Implemented

- **Custom Prompt**: Displays the current working directory as the prompt, labeled as `PUCITshell`.
- **Command Execution**: Supports executing standard UNIX commands, such as `ls`, `pwd`, `echo`, and more.
- **Custom Command History**:
  - Maintains a limited history of the last 10 commands.
  - Saves command history to `.myshell_history`, enabling persistence across sessions.
  - Up and down arrow keys can be used to navigate through the last 10 commands in the custom history.
- **Command Repetition**:
  - Allows users to repeat previously issued commands using `!number` syntax (e.g., `!1` for the first command).
  - Supports `!-1` to repeat the last command in the history.
- **Exit Condition**: Exits the shell gracefully when `CTRL+D` is pressed.

### File Structure

- `shell_v4.c`: Contains the complete code for Version 04, including:
  - Displaying the prompt.
  - Parsing input and executing commands.
  - Custom history management with up/down arrow navigation and command repetition.
  - Tab completion for basic commands.

### Known Issues and Errors

- **History Limit**: The custom history only keeps track of the last 10 commands. New commands overwrite the oldest entries in memory and in `.myshell_history`.
- **Error Handling**: Basic error messages are displayed if commands fail or if the `fork()` system call does not work. Limited handling for specific errors, such as command syntax issues.

### Limitations

- **Single History File**: Only one `.myshell_history` file is used, maintaining only the last 10 commands. Commands exceeding this limit will overwrite older entries.
- **No Redirection or Piping**: Input/output redirection (e.g., `<`, `>`) and pipes (`|`) are not implemented in this version, which limits command chaining functionality.
- **Basic Error Handling**: While errors are handled for command execution failures, edge cases and complex errors may not be fully addressed.

### Acknowledgments

This project was developed with guidance and support from:

- **Arif Butt**: My instructor, who provided valuable insights into UNIX system programming concepts. GitHub Profile: [Arif Butt](https://github.com/arifpucit)
- **ChatGPT by OpenAI**: Assisted in structuring the code, resolving challenges with `readline`, and implementing the custom history system.
- UNIX system programming guides and online documentation for system calls.

