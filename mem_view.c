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

struct mem_buf {
  struct heap_stack* mem_info;
  char* old_buf;
  char* new_buf;
};

void free_buf(struct mem_buf* buf)
{
  free(buf->mem_info);
  free(buf->new_buf);
  free(buf);
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
  struct mem_buf* mb = malloc(sizeof(struct mem_buf));
  mb->mem_info = parse_addr(addrs);
  
  char* mem_path = cat_path(args, argv, "/mem");
  FILE* mem = fopen(mem_path, "r");
  free(mem_path);
  if (mem == NULL) {
    perror("Open mem failed");
    return 1;
  }
  
  parse_command(args, argv, FLAG);
  mb->old_buf = parse_mem(mem, mb->mem_info, FLAG);
  long int buf_size  = strcmp(FLAG, "stack") == 0 ? mb->mem_info->stack_size : mb->mem_info->heap_size;
  long int row_range = buf_size / (ws->ws_row - 2);
  long int uni_range = row_range / ws->ws_col;
  long int curr_range= uni_range;
  printf("\033[H\033[J\033[?25l");
  printf("mem_view refresh in 0.5s.\n");
  while (! stop) {
    mb->new_buf = strcmp(FLAG, "stack") == 0 ?
      parse_mem(mem, mb->mem_info, "stack") : parse_mem(mem, mb->mem_info, "heap");
    buf_size  = strcmp(FLAG, "stack") == 0 ? mb->mem_info->stack_size : mb->mem_info->heap_size;
    for (int i = 0; i < buf_size; i += uni_range) {
      curr_range = i + uni_range > buf_size ? buf_size - i : uni_range;
      if (memcmp(mb->old_buf + i, mb->new_buf + i, curr_range) != 0) {
	printf("\033[%d;%dH\033[31mX", (i / row_range) + 2, (i % row_range) / uni_range + 1);
      } else {
	printf("\033[%d;%dH\033[37m|", (i / row_range) + 2, (i % row_range) / uni_range + 1);
      }
    }
    free(mb->old_buf);
    mb->old_buf = mb->new_buf;
    fflush(stdout);
    usleep(500000);
  }
  free_buf(mb);
  free(ws);
  fclose(mem);
  printf("\033[H\033[J\033[?25h");
  fflush(stdout);
  return 0;
}
