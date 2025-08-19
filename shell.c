#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"

void shell();
void shell_cycle();
void process_command(char command[]);
void print_cwd();
void clear_terminal();
void handle_cd(char *tokens[]);
void handle_pipeline(char command[]);
void handle_redirection(char command[]);
void execute_simple_command(char *tokens[]);

int main(void) {
    shell();
    return 0;
}

void shell() {
    clear_terminal();
    shell_cycle();
    return;
}

void shell_cycle() {
    char command[1024];
    print_cwd();
    for (;;) {
        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("\n");
            exit(0);
        }
        command[strcspn(command, "\n")] = '\0';
        process_command(command);
        print_cwd();
    }
}

void process_command(char command[]) {
    if (strcmp(command, "clear") == 0) {
        clear_terminal();
        return;
    }

    if (strcmp(command, "exit") == 0) {
        exit(0);
    }

    if (strchr(command, '|') != NULL) {
        handle_pipeline(command);
        return;
    }

    if (strchr(command, '>') != NULL || strchr(command, '<') != NULL) {
        handle_redirection(command);
        return;
    }

    char *tokens[20];
    int count = 0;
    char *token = strtok(command, " ");
    
    while (token != NULL && count < 19) {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }
    tokens[count] = NULL;

    if (strcmp(tokens[0], "cd") == 0) {
        handle_cd(tokens);
        return;
    }

    execute_simple_command(tokens);
}

void execute_simple_command(char *tokens[]) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        execvp(tokens[0], tokens);
        perror("shell: command not found");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void print_cwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s%s %s$%s ", CYAN, cwd, MAGENTA, RESET);
    } else {
        perror("getcwd() error");
    }
    fflush(stdout);
}

void handle_cd(char *tokens[]) {
    char *path;
    if (tokens[1] == NULL) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "HOME environment variable not set\n");
            return;
        }
    } else {
        path = tokens[1];
    }

    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

void clear_terminal() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

// TODO: 
void handle_pipeline(char command[]) {}

void handle_redirection(char command[]) {
    char *tokens[20];
    int count = 0;
    
    char *token = strtok(command, " ");
    while (token != NULL && count < 19) {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }
    tokens[count] = NULL;

    char *input_file = NULL;
    char *output_file = NULL;
    int append_mode = 0;
    int new_count = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(tokens[i], "<") == 0 && i + 1 < count) {
            input_file = tokens[i + 1];
            i++; 
        }
        else if (strcmp(tokens[i], ">") == 0 && i + 1 < count) {
            output_file = tokens[i + 1];
            i++;
        }
        else if (strcmp(tokens[i], ">>") == 0 && i + 1 < count) {
            output_file = tokens[i + 1];
            append_mode = 1;
            i++;
        }
        else {
            tokens[new_count++] = tokens[i];
        }
    }
    tokens[new_count] = NULL;

    if (new_count == 0) {
        fprintf(stderr, "No command specified\n");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        if (input_file != NULL) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("open input file");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        
        if (output_file != NULL) {
            int flags = O_WRONLY | O_CREAT;
            if (append_mode) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }
            
            int fd_out = open(output_file, flags, 0644);
            if (fd_out == -1) {
                perror("open output file");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        
        execvp(tokens[0], tokens);
        perror("execvp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}
