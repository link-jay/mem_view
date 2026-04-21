#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

static struct termios old_term;
static int old_flags;

void term_init(void)
{
  tcgetattr(STDIN_FILENO, &old_term);
  struct termios new_term = old_term;
  new_term.c_lflag &= ~(ICANON | ECHO);
  new_term.c_cc[VMIN] = 0;
  new_term.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
  old_flags = fcntl(STDIN_FILENO, TCSANOW, &new_term);
  old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);
}

void term_restore(void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
  fcntl(STDIN_FILENO, F_SETFL, old_flags);
}

int term_getkey(void)
{
  unsigned char c;
  if (read(STDIN_FILENO, &c, 1) == 1) {
    return c;
  }
  return -1;
}

void prepare_terminal()
{
  printf("\033[H\033[J\033[?25l");
}

void restore_terminal()
{
  printf("\033[H\033[J\033[?25h");
}
