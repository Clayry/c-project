#include "../include/snake.h"
#include <ncurses.h>
#include <stdbool.h>

void input_handle(Game *g) {
  int ch = getch();
  if (ch == ERR) {
    return;
  }

  switch (ch) {
  case KEY_UP:
  case 'w':
  case 'W':
    g->snake.next_dir = UP;
    break;
  case KEY_DOWN:
  case 's':
  case 'S':
    g->snake.next_dir = DOWN;
    break;
  case KEY_LEFT:
  case 'a':
  case 'A':
    g->snake.next_dir = LEFT;
    break;
  case KEY_RIGHT:
  case 'd':
  case 'D':
    g->snake.next_dir = RIGHT;
    break;

  case 'p':
  case 'P':
    g->paused = !g->paused;
    break;

  case 'q':
  case 'Q':
    g->running = false;
    break;

  default:
    break;
  }
}
