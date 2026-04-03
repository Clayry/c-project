#define _XOPEN_SOURCE 600
#include "../include/snake.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static void sleep_ms(int ms) { usleep((unsigned int)(ms * 1000)); }

int main(void) {
  srand((unsigned)time(NULL));

  render_init();

  Game g;

restart:
  game_init(&g);

  while (1) {
    if (g.running) {
      input_handle(&g);
      game_update(&g);
      render_frame(&g);
      sleep_ms(g.speed_ms);
    } else {
      render_frame(&g);
      render_gameover(&g);

      nodelay(stdscr, FALSE);
      int ch = getch();
      nodelay(stdscr, TRUE);

      if (ch == 'r' || ch == 'R')
        goto restart;
      break;
    }
  }

  render_cleanup();
  return 0;
}
