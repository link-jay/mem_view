#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int main()
{
  
  int i = 2101;
  const int j = -189223;
  int* k = malloc(sizeof(int));
  
  for (;; i++) {
    *k = (i -2101) << 1;
    printf("[%d]\n"
	   "string: hello, world.\n"
	   "i32 is %d.\n"
	   "heap_test %d\n"
	   "--------------------\n",
	   i, j, *k);
    sleep(2);
  }
  
  return 0;
}
