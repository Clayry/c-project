#include "../include/tetris.h"
#include <ncurses.h>

void input_handle(Game *g) {
  int ch = getch();
  if (ch == ERR)
    return;

  switch (ch) {
  case KEY_LEFT:
  case 'a':
  case 'A':
    game_move(g, -1);
    break;
  case KEY_RIGHT:
  case 'd':
  case 'D':
    game_move(g, +1);
    break;
  case KEY_UP:
  case 'w':
  case 'W':
    game_rotate(g);
    break;
  case KEY_DOWN:
  case 's':
  case 'S':
    game_soft_drop(g);
    break;
  case ' ':
    game_hard_drop(g);
    break;
  case 'p':
  case 'P':
    g->paused = !g->paused;
    break;
  case 'q':
  case 'Q':
    g->running = false;
    g->game_over = true;
    break;
  default:
    break;
  }
}
