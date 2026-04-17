#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char FLAG[];
extern size_t REFRESH_TIME;

void parse_command(int args, char* argv[])
{
  char help_message[256] = \
    "-h: \tview heap"
    "-s: \tview stack"
    "-t: \tset refresh time"
    "--help: show this message";
  for (int i = 1; i < (args - 1); i++) {
    if (strcmp(argv[i], "-h") == 0) {
      strcpy(FLAG, "heap");
    }
    else if (strcmp(argv[i], "-s") == 0) {
      strcpy(FLAG, "stack");
    }
    else if (strcmp(argv[i], "-t") == 0) {
      REFRESH_TIME = strtoul(argv[++i], NULL, 10) * 1000;
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
