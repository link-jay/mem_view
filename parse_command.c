#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char FLAG[];
extern size_t REFRESH_TIME;

void parse_command(int args, char* argv[])
{
  char help_message[256] = \
    "-s:\tView stack (default).\n"
    "-h:\tView heap.\n"
    "-t <time>:\n\tSet refresh interval (default: 500ms).\n"
    "--help:\tShow help message.\n"
    "While running:\n"
    "- Type s/h to switch between stack/heap view.\n"
    "- Type q or press Ctrl-C to quit.\n";
  if (strcmp(argv[args - 1], "--help") == 0) {
    printf("%s\n", help_message);
    exit(0);
  }
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
    else {
      printf("Unkown argv %s, please use --help to check.", argv[i]);
      exit(1);
    }
  }
}
