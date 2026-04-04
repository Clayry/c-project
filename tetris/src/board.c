#include "../include/tetris.h"
#include <stdbool.h>
#include <string.h>

void board_init(Game *g) { memset(g->board, 0, sizeof(g->board)); }

int board_clear_line(Game *g) {
  int cleared = 0;

  for (int r = BOARD_H - 1; r >= 0; r--) {
    bool full = true;
    for (int c = 0; c < BOARD_W; c++) {
      if (!g->board[r][c]) {
        full = false;
        break;
      }
    }
    if (!full)
      continue;

    for (int above = r; above > 0; above--) {
      memcpy(g->board[above], g->board[above - 1], sizeof(g->board[0]));
    }

    memset(g->board[0], 0, sizeof(g->board[0]));

    cleared++;
    r++;
  }

  return cleared;
}
