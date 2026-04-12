#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_command(int args, char* argv[], char* flag)
{
  char help_message[256] = \
    "-h: \tview heap"
    "-s: \tview stack"
    "--help: show this message";
  for (int i = 1; i < (args - 1); i++) {
    if (strcmp(argv[i], "-h") == 0) {
      strcpy(flag, "heap");
    }
    else if (strcmp(argv[i], "-s") == 0) {
      strcpy(flag, "stack");
    }
    else if (strcmp(argv[i], "--help") == 0) {
      printf("%s\n", help_message);
      exit(0);
    }
    else {
      printf("Unkown argv %s, please use --help to check.", argv[i]);
      exit(1);
    }
  }
}
