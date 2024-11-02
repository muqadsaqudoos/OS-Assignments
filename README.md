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



