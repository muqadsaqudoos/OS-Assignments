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

## Custom UNIX Shell - Version 02

This project is a custom UNIX-like shell created as a learning exercise to understand UNIX system calls, command redirection, and piping. This version adds functionality for input/output redirection (`<` and `>`) and pipes (`|`).

### Features Implemented (Version 02)

- **Custom Prompt**: Shows the current working directory as the prompt, labeled as `PUCITshell`.
- **Basic Command Execution**: Supports running standard UNIX commands (e.g., `ls`, `pwd`, `echo`).
- **Input/Output Redirection**:
  - `<` for redirecting input from a file.
  - `>` for redirecting output to a file.
  - Example: `cat < infile.txt > outfile.txt`
- **Pipes**:
  - Allows chaining commands so that the output of one command becomes the input of another.
  - Example: `ls | wc -l`

### Known Issues and Errors

- **Commands like `grep` and `wc` in pipes**: When using commands like `grep` and `wc` with pipes, some combinations may not work correctly. For example, `cat input.txt | wc -w` might encounter issues in this shell. This is a known limitation, and further debugging is needed to fully support these commands in complex pipelines.
- **Error Messages**: Basic error handling is in place, showing messages if a command fails to execute or if a file can’t be opened.

### File Structure

- `shell_v2.c`: Contains all the code for Version 02, handling the main shell loop, input parsing, command execution, redirection, and piping.


### Testing Commands

Here are some sample commands to test the shell's functionality:

1. **Basic Commands**:
   - `ls`
   - `echo "Hello, World"`

2. **Input Redirection**:
   - `cat < input.txt` (reads from `input.txt` and displays content)

3. **Output Redirection**:
   - `echo "Testing output" > output.txt` (writes "Testing output" to `output.txt`)

4. **Combined Input and Output Redirection**:
   - `cat < input.txt > output_copy.txt` (copies content from `input.txt` to `output_copy.txt`)

5. **Simple Pipe**:
   - `ls | wc -l` (counts files in the directory)
   
6. **Complex Pipe**:
   - `cat input.txt | grep sample | wc -l` (finds lines containing "sample" in `input.txt` and counts them)
   
**Note**: Some commands like `grep` and `wc` may have issues when used in pipelines, especially in combination. This is a known limitation.


