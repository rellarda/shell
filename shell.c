#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int main(void) {
  shell();
  return 0;
}

void shell() {
  system("clear");
  shell_cycle();
  return;
}

void shell_cycle() {
  char command[1024];
  print_cwd();
  for (;;) {
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';
    process_command(command);
    print_cwd();
  }

  return;
}

void process_command(char command[]) {
  char *tokens[10];

  int count = 0;
  char *token = strtok(command, " ");
  while (token != NULL && count < 10) {
    tokens[count++] = token;
    token = strtok(NULL, " ");
  }
  tokens[count] = NULL;

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork failed");
    exit(1);
  }

  if (pid == 0) {              // Дочерний процесс
    execvp(tokens[0], tokens); // Запускаем команду
    perror("execvp failed");   // Если execvp вернул ошибку
    exit(1);
  } else { // Родительский процесс
    int status;
    waitpid(pid, &status, 0); // Ждём завершения
  }

  return;
}

void print_cwd() {
  char cwd[256];
  if (getcwd(cwd, sizeof(cwd))) {
    printf("%s%s %s$%s ", CYAN, cwd, MAGENTA, RESET);
  } else {
    perror("getcwd() error");
    return;
  }
}
