# OS Assignment 1

## My Custom Shell - A UNIX-like Command Line Interface

This project is a UNIX-like shell developed as an assignment for an Operating Systems course. The shell was progressively enhanced through multiple versions, each adding new functionality and capabilities using C and UNIX system calls.

### Table of Contents
- [Overview](#overview)
- [File Structure](#file-structure)
- [Features by Version](#features-by-version)
  - [Version 01](#version-01)
  - [Version 02](#version-02)
  - [Version 03](#version-03)
  - [Version 04](#version-04)
  - [Version 05](#version-05)
  - [Version 06](#version-06)
- [Known Issues and Limitations](#known-issues-and-limitations)
- [Acknowledgments](#acknowledgments)

---

## Overview

This project is a custom shell developed in C using UNIX system calls. Starting with a basic shell prompt, each version builds upon the last by introducing features like command execution, redirection, background processing, and variable management. This incremental approach allowed for systematic learning and enhancement of the shell's functionality.

## File Structure

- **`shell_v1.c`**: Contains the code for Version 01, covering basic prompt display and command execution.
- **`shell_v2.c`**: Contains the code for Version 02, adding input/output redirection and piping functionality.
- **`shell_v3.c`**: Contains the code for Version 03, introducing background command execution.
- **`shell_v4.c`**: Contains the code for Version 04, implementing custom command history with navigation and repetition.
- **`shell_v5.c`**: Contains the code for Version 05, incorporating built-in commands and background job management.
- **`shell_v6.c`**: Contains the code for Version 06, adding user-defined variables and environment variable management.
- **`.myshell_history`**: Stores the last 10 commands, enabling persistence of command history across sessions.
- **`README.md`**: This README file, providing an overview of the project, features, and known issues.

## Features by Version

### Version 01

**Features:**
- **Custom Prompt**: Displays the current working directory as `PUCITshell@<directory>:-`, providing a visual indicator of the current location.
- **Basic Command Execution**: Supports executing standard UNIX commands entered by the user, such as `ls`, `pwd`, and `echo`.
- **Exit Condition**: Allows users to exit the shell by pressing `CTRL+D`.

**Known Issues:**
- **No Redirection or Piping**: Input/output redirection (e.g., `<`, `>`) and piping (e.g., `|`) are not implemented in this version.
- **Error Handling**: Basic error messages are displayed if commands fail or if system calls like `fork()` do not execute successfully.

---

### Version 02

**Features:**
- **Custom Prompt**: Same as in Version 01.
- **Command Execution**: Supports executing standard UNIX commands entered by the user.
- **Input and Output Redirection**:
  - Supports `<` for input redirection from a file and `>` for output redirection to a file.
  - Example: `cat < input.txt > output.txt` reads from `input.txt` and writes to `output.txt`.
- **Piping**: Allows command chaining with a single pipe (`|`), where the output of one command serves as input to another.
  - Example: `ls | wc -l` lists files in the directory and counts them.
- **Exit Condition**: Same as in Version 01.

**Known Issues:**
- **Single Pipe Limitation**: Only a single `|` is supported. Multi-piping (e.g., `ls | grep txt | wc -l`) is not yet implemented.
- **File Descriptor Management**: Properly closes unused file descriptors to avoid resource leaks and system hang-ups, but advanced cases may not be handled.

---

### Version 03

**Features:**
- **Background Command Execution**: Commands can be run in the background by appending `&` to the end of a command.
  - Example: `find / -name file.txt &` will execute `find` in the background, allowing the user to continue using the shell.
- **Process IDs for Background Jobs**: Background jobs display their process IDs (PIDs) in the format `[1] PID`, making it easy to identify running processes.

**Known Issues:**
- **Limited Background Job Management**: Basic background processing is supported, but more advanced job control features, such as `fg` (foreground) and `bg` (background), are not implemented.

---

### Version 04

**Features:**
- **Custom Command History**:
  - Maintains a history of the last 10 commands, accessible by typing `history`.
  - Saves command history to `.myshell_history`, allowing persistence across sessions.
  - Up and down arrow keys allow users to navigate through the last 10 commands.
- **Command Repetition**:
  - Repeats specific commands using `!number` syntax (e.g., `!1` for the first command in history).
  - Supports `!-1` to repeat the last command entered.

**Known Issues:**
- **History Limit**: Only the last 10 commands are retained. Commands exceeding this limit overwrite the oldest entry in memory and in `.myshell_history`.
- **Basic Error Handling**: Error messages for command repetition are basic, and advanced cases may not be covered.

---

### Version 05

**Features:**
- **Built-in Commands**:
  - **`cd <path>`**: Changes the current directory to the specified path.
  - **`exit`**: Exits the shell.
  - **`jobs`**: Lists active background jobs, showing job numbers, PIDs, and commands.
  - **`kill <PID>`**: Terminates a background job by its PID.
  - **`help`**: Lists available built-in commands and their syntax.
- **Background Job Management**:
  - Allows users to view active background jobs using the `jobs` command.
  - Terminate background jobs using `kill <PID>`.
  - Displays job numbers, PIDs, and commands for each background job.
- **Custom Command History**: Similar to Version 04, with navigation and repetition of previous commands.
- **Exit Condition**: Same as in previous versions, allowing graceful exit on `CTRL+D`.

**Known Issues:**
- **Error Handling**: Limited error handling is provided for syntax errors in job management commands (e.g., invalid `kill` commands).
- **Basic Built-in Commands**: Although background job management is introduced, advanced job control (e.g., `fg` and `bg` for job re-assignment) is not implemented.
- **No Redirection or Piping**: Redirection and piping are not supported in this version.

---

### Version 06

**Features:**
- **User-defined and Environment Variables**:
  - Allows users to define local shell variables using the syntax `name=value`.
  - Example: `myvar=hello` creates a variable `myvar` with value `hello`.
- **Environment Variable Export**:
  - Supports the `export` command to make local variables available as environment variables.
  - Example: `export myvar` makes `myvar` accessible as an environment variable.
- **Variable Expansion**:
  - Allows accessing the value of defined variables using `$name` syntax in commands.
  - Example: `echo $myvar` outputs the value of `myvar`.
- **Unsetting Variables**:
  - Supports the `unset` command to remove variables from the shell environment.
  - Example: `unset myvar` deletes the variable `myvar`.

**Known Issues:**
- **Basic Variable Handling**: Supports simple variables but does not support complex data types or advanced shell variable features.
- **Exported Variables Persistence**: Exported variables only persist for the shell session; they do not save to the environment permanently.

---

## Known Issues and Limitations

Across all versions, the following limitations apply:

1. **Limited Command Chaining**: Only a single `|` is supported in commands, and chaining multiple pipes (e.g., `ls | grep txt | wc -l`) is not implemented.
2. **Error Handling**: While basic error handling is present, more informative messages for edge cases and syntax errors are limited.
3. **History Persistence**: The `.myshell_history` file only stores the last 10 commands, which are overwritten when the limit is exceeded.
4. **No Advanced Job Control**: Job management commands like `fg` (foreground) and `bg` (background) are not implemented, limiting control over background processes.
5. **No Multi-level Export**: Exported variables only last for the shell session and are not saved to the environment for future sessions.

## Acknowledgments

This project was developed with the support and guidance of:

- **Arif Butt**: My instructor, who provided valuable insights into UNIX system programming concepts. GitHub Profile: [Arif Butt](https://github.com/arifpucit)
- **ChatGPT by OpenAI**: Assisted in structuring the code, implementing features like background processing, job management, variable handling, and command repetition, as well as troubleshooting issues with UNIX system calls.
- **UNIX System Programming Guides
