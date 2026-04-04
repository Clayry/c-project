#include "../include/tetris.h"
#include <ncurses.h>
#include <stdbool.h>

#define CP_BORDER 8
#define CP_GHOST 9
#define CP_UI 10

void render_init(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);
  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(CP_BORDER, COLOR_WHITE, COLOR_BLACK);
    init_pair(CP_GHOST, COLOR_BLACK, COLOR_BLACK);
    init_pair(CP_UI, COLOR_YELLOW, COLOR_BLACK);
  }
}

void render_cleanup(void) { endwin(); }

static void draw_border(void) {
  attron(COLOR_PAIR(CP_BORDER));
  int x0 = BOARD_OFF_X - 1;
  int y0 = BOARD_OFF_Y - 1;
  int x1 = BOARD_OFF_X - BOARD_W * 2;
  int y1 = BOARD_OFF_Y + BOARD_H;
  for (int x = x0; x <= x1; x++) {
    mvaddch(y0, x, ACS_HLINE);
    mvaddch(y1, x, ACS_HLINE);
  }
  for (int y = y0; y <= y1; y++) {
    mvaddch(y, x0, ACS_VLINE);
    mvaddch(y, x1, ACS_VLINE);
  }
  mvaddch(y0, x0, ACS_ULCORNER);
  mvaddch(y0, x1, ACS_URCORNER);
  mvaddch(y1, x0, ACS_LLCORNER);
  mvaddch(y1, x1, ACS_LRCORNER);
  attroff(COLOR_PAIR(CP_BORDER));
}

static void draw_cell(int row, int col, int cp) {
  int sx = BOARD_OFF_X + col * 2;
  int sy = BOARD_OFF_Y + row;
  attron(COLOR_PAIR(cp) | A_REVERSE);
  mvaddstr(sy, sx, "\t");
  attroff(COLOR_PAIR(cp) | A_REVERSE);
}

static Piece compute_ghost(const Game *g) {
  Piece ghost = g->current;
  while (1) {
    Piece below = ghost;
    below.y++;
    if (!piece_fits(g, &below))
      break;
    ghost = below;
  }
  return ghost;
}

static void draw_piece_at(const Game *g, const Piece *p, int cp) {}
