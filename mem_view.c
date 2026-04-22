#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "readmem.c"
#include "parse_command.c"
#include "term_key.c"

volatile sig_atomic_t stop = 0;
char FLAG[8];
size_t REFRESH_TIME = 500 * 1000;

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
  parse_command(args, argv);
  
  strcpy(FLAG, "stack");
  signal(SIGINT, sigint_handler);
  
  struct winsize* ws;

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
  
  prepare_terminal();
  term_init();
  long int buf_size, row_range, uni_range, curr_range;
  int key;
  mb->old_buf = NULL;
  while (! stop) {
    /* section deal change terminal size */
    ws = get_tsz();
    buf_size  = strcmp(FLAG, "stack") == 0 ? mb->mem_info->stack_size : mb->mem_info->heap_size;
    row_range = buf_size / (ws->ws_row - 2);
    uni_range = row_range / ws->ws_col;
    mb->new_buf = parse_mem(mem, mb->mem_info, FLAG);
    /* section draw view */
    printf("\033[H%s refresh in %.3fs. Each unit for %dB.\n", FLAG, REFRESH_TIME / (1000 * 1000.0), uni_range);
    if (mb->old_buf != NULL) {
      for (int i = 0; i < buf_size; i += uni_range) {
	curr_range = i + uni_range > buf_size ? buf_size - i : uni_range;
	if (memcmp(mb->old_buf + i, mb->new_buf + i, curr_range) != 0) {
	  printf("\033[%d;%dH\033[31mX", (i / row_range) + 2, (i % row_range) / uni_range + 1);
	} else {
	  printf("\033[%d;%dH\033[37m|", (i / row_range) + 2, (i % row_range) / uni_range + 1);
	}
      }
    } else {
      puts("\033[H\033[Jwaiting...");
    }
    fflush(stdout);
    usleep(REFRESH_TIME);
    free(mb->old_buf);
    /* section hotkey */
    key = term_getkey();
    if (key != -1) {
      if (key == 's') {strcpy(FLAG, "stack"); mb->old_buf = NULL; free(mb->new_buf);}
      else if (key == 'h') {strcpy(FLAG, "heap"); mb->old_buf = NULL; free(mb->new_buf);}
      else if (key == 'q') {stop = 1;}
    } else {
      mb->old_buf = mb->new_buf;
    }
    free(ws);
  }
  free_buf(mb);
  fclose(mem);
  term_restore();
  restore_terminal();
  return 0;
}
