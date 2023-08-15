#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 64
#define MAX_HISTORY 10

void execute_command(char *command, char history[MAX_HISTORY][MAX_COMMAND_LENGTH], int * history_count) {
    
    if (strcmp(command, "exit") == 0) {
        // Exit the shell
        printf("Exiting shell...\n");
        return;
    } else if (strncmp(command, "cd", 2) == 0) {

        // Change directory command
        char *args[MAX_ARGUMENTS];
        int arg_count = 0;

        // Tokenize the command
        char *token = strtok(command, " ");
        while (token != NULL && arg_count < MAX_ARGUMENTS - 1) {
            args[arg_count] = token;
            arg_count++;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (arg_count == 1) {
            printf("Usage: cd <directory>\n");
        } else if (arg_count > 2) {
            printf("cd: Too many arguments\n");
        } else {
            if (chdir(args[1]) == -1) {
                perror("chdir");
            }
        }
    } else if (strcmp(command, "history") == 0) {
        printf("Command history:\n");
        for (int i = 0; i < *history_count; i++) {
            printf("%d. %s\n", i + 1, history[i]);
        }
    }
    else{
        pid_t child_pid;
        int status;

        child_pid = fork();
        
        if (child_pid == 0) {
            // Child process
            char *args[MAX_ARGUMENTS];
            int arg_count = 0;

            // Tokenize the command
            char *token = strtok(command, " ");
            while (token != NULL && arg_count < MAX_ARGUMENTS - 1) {
                args[arg_count] = token;
                arg_count++;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL;

            execvp(args[0], args);  // Execute the command
            perror("execvp");       // If execvp fails
            exit(EXIT_FAILURE);
        } else if (child_pid > 0) {
            // Parent process
            waitpid(child_pid, &status, 0);  // Wait for child to finish
        } else {
            perror("fork");  // If fork fails
        }
    } 
}

void update_history(char history[MAX_HISTORY][MAX_COMMAND_LENGTH], int *history_count, const char *command) {
    if (*history_count < MAX_HISTORY) {
        strcpy(history[*history_count], command);
        (*history_count)++;
    } else {
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(history[i], history[i + 1]);
        }
        strcpy(history[MAX_HISTORY - 1], command);
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char history[MAX_HISTORY][MAX_COMMAND_LENGTH];
    int history_count = 0;
    
    while (1) {
        printf("MTL458 > ");  // Print the command prompt
        
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            // Handle Ctrl+C or EOF
            printf("\nExiting shell...\n");
            break;
        }

        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';
        update_history(history, &history_count, command);

        int pipe_index = -1;
        for (int i = 0; i < strlen(command); i++) {
            if (command[i] == '|') {
                pipe_index = i;
                break;
            }
        }
        
        if (pipe_index >= 0) {
            char first_command[MAX_COMMAND_LENGTH];
            char second_command[MAX_COMMAND_LENGTH];
            
            strncpy(first_command, command, pipe_index);
            first_command[pipe_index] = '\0';
            
            strcpy(second_command, command + pipe_index + 1);

            // Create a pipe
            int fd[2];
            if (pipe(fd) == -1) {
                perror("pipe");
            }

            // Execute the first command in a child process
            if (fork() == 0) {
                close(fd[0]);  // Close read end of the pipe
                dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
                close(fd[1]);
                execute_command(first_command, history, &history_count);
                exit(EXIT_SUCCESS);
            }

            // Execute the second command in the parent process
            if (fork() == 0) {
                close(fd[1]);  // Close write end of the pipe
                dup2(fd[0], STDIN_FILENO);  // Redirect stdin from the pipe
                close(fd[0]);
                execute_command(second_command, history, &history_count);
                exit(EXIT_SUCCESS);
            }

            close(fd[0]);
            close(fd[1]);

            // Wait for both child processes to finish
            wait(NULL);
            wait(NULL);
        } else {
            execute_command(command, history, &history_count);
        }
    }
    
    return 0;
}