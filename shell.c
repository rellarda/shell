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
  for (;;) {
    print_cwd();
    fgets(command, sizeof(command), stdin);
    process_command(command);
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

  for (int i = 0; i < count; i++) {
    printf("Token %d: %s\n", i, tokens[i]);
  };
  return;
}

void print_cwd() {
  char cwd[256];
  if (getcwd(cwd, sizeof(cwd))) {
    printf("%s%s %s$%s\n", CYAN, cwd, MAGENTA, RESET);
  } else {
    perror("getcwd() error");
    return;
  }
}
