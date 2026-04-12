#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "readmem.c"
#include "parse_command.c"

volatile sig_atomic_t stop = 0;

struct winsize* get_tsz()
{
  struct winsize* ws = malloc(sizeof(struct winsize));
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, ws) == -1) {
    perror("get terminal size failed\n");
    exit(1);
  }
  return ws;
}

void draw_view(size_t rows, size_t cols)
{
  printf("\033[?25l\033[H\033[2J");
  printf("mem_view flush in 0.5s\n");
  for (size_t r = 0; r < rows - 2; r++) {
    for (size_t c = 0; c < cols; c++) {
      putchar('|');
    }
    putchar('\n');
  }
  fflush(stdout);
}

void sigint_handler(int sig)
{
  stop = 1;
}

int main(int args, char* argv[])
{
  char* FLAG = malloc(8);
  strcpy(FLAG, "stack");
  signal(SIGINT, sigint_handler);
  struct winsize* ws = get_tsz();

  char* maps_path = cat_path(args, argv, "/maps");
  FILE* maps = fopen(maps_path, "r");
  free(maps_path);
  if (maps == NULL) {
    perror("Open maps failed");
    return 1;
  }
  char* addrs = parse_maps(maps);
  fclose(maps);
  struct heap_stack* mem_info = parse_addr(addrs);
  
  char* mem_path = cat_path(args, argv, "/mem");
  FILE* mem = fopen(mem_path, "r");
  free(mem_path);
  if (mem == NULL) {
    perror("Open mem failed");
    return 1;
  }
  
  parse_command(args, argv, FLAG);
  char *old_buf, *new_buf;
  old_buf = parse_mem(mem, mem_info, FLAG);
  long int buf_size  = strcmp(FLAG, "stack") == 0 ?
    mem_info->stack_end - mem_info->stack_start : mem_info->heap_end - mem_info->heap_start;
  long int row_range = buf_size / (ws->ws_row - 2);
  long int uni_range = row_range / ws->ws_col;
  long int curr_range= uni_range;
  draw_view(ws->ws_row, ws->ws_col);
  while (! stop) {
    new_buf = strcmp(FLAG, "stack") == 0 ?
      parse_mem(mem, mem_info, "stack") : parse_mem(mem, mem_info, "heap");
    buf_size  = strcmp(FLAG, "stack") == 0 ?
      mem_info->stack_end - mem_info->stack_start : mem_info->heap_end - mem_info->heap_start;
    for (int i = 0; i < buf_size; i += uni_range) {
      curr_range = i + uni_range > buf_size ? buf_size - i : uni_range;
      if (memcmp(old_buf + i, new_buf + i, curr_range) != 0) {
	printf("\033[%d;%dH\033[31mX", (i / row_range) + 2, (i % row_range) / uni_range + 1);
      } else {
	printf("\033[%d;%dH\033[37m|", (i / row_range) + 2, (i % row_range) / uni_range + 1);
      }
    }
    free(old_buf);
    old_buf = new_buf;
    fflush(stdout);
    usleep(500000);
  }
  free(new_buf);
  free(mem_info);
  free(ws);
  fclose(mem);
  printf("\033[H\033[J\033[?25h");
  fflush(stdout);
  return 0;
}
