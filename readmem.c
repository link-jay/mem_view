#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include <unistd.h> */

char* cat_path(int args, char* argv[], char* file_name)
{
  char* path = calloc(32, sizeof(char));
  strcat(path, "/proc/");
  strcat(path, argv[args - 1]);
  strcat(path, file_name);
  return path;
}

char* parse_maps(FILE* maps) {
  char* result = calloc(1024, sizeof(char));
  char* line = NULL;
  size_t len = 0;
  while (getline(&line, &len, maps) != -1) {
    if (strstr(line, "heap") != NULL || strstr(line, "stack") != NULL) {
      strcat(result, line);
    }
  }
  free(line);
  return result;
}

struct heap_stack {
  long int heap_start;
  long int heap_end;
  long int heap_size;
  long int stack_start;
  long int stack_end;
  long int stack_size;
};

struct heap_stack* parse_addr(char* addr)
{
  /*
    55d51dad0000-55d51daf1000 rw-p 00000000 00:00 0                          [heap]
    55d51dad0000-55d51daf1000 rw-p 00000000 00:00 0                          [stack]
  */
  struct heap_stack* result = malloc(sizeof(struct heap_stack));
  sscanf(addr, "%lx-%lx %*[^\n]" "%lx-%lx %*[^\n]",
	 &result->heap_start,
	 &result->heap_end,
	 &result->stack_start,
	 &result->stack_end);
  return result;
}

char* parse_mem(FILE* mem, struct heap_stack* mem_info, const char* mode)
{
  if (strcmp("heap", mode) == 0) {
    mem_info->heap_size = mem_info->heap_end - mem_info->heap_start;
    char* buf = malloc(mem_info->heap_size);
    fseek(mem, mem_info->heap_start, SEEK_SET);
    fread(buf, 1, mem_info->heap_size, mem);
    return buf;
  } else if (strcmp("stack", mode) == 0) {
    mem_info->stack_size = mem_info->stack_end - mem_info->stack_start;
    char* buf = malloc(mem_info->stack_size);
    fseek(mem, mem_info->stack_start, SEEK_SET);
    fread(buf, 1, mem_info->stack_size, mem);
    return buf;
  } else{
    printf("`%s` mode is not allow", mode);
    exit(1);
  }
}
