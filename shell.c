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

  char cwd[256];
  if (getcwd(cwd, sizeof(cwd))) {
    printf("%s%s %s$%s\n", CYAN, cwd, MAGENTA, RESET);
  } else {
    perror("getcwd() error");
    return;
  }

  char command[1024];

  for (;;) {
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "start") == 0) {
      printf("Запуск программы...\n");
    } else if (strcmp(command, "stop") == 0) {
      printf("Остановка программы...\n");
    } else if (strcmp(command, "help") == 0) {
      printf("Справка: ...\n");
    } else if (strcmp(command, "exit") == 0) {
      return;
    } else {
      printf("Неизвестная команда!\n");
    }

    printf("%s%s %s$%s\n", CYAN, cwd, MAGENTA, RESET);
  }

  return;
}
