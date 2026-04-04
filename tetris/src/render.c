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

static void draw_piece_at(const Game *g, const Piece *p, int cp) {
  int t = (int)p->type;
  int rot = p->rot;
  for (int r = 0; r < PIECE_SIZE; r++) {
    int mask = shapes[t][rot][r];
    for (int c = 0; c < PIECE_SIZE; c++) {
      if (!(mask & (0x8 >> c)))
        continue;
      int bx = p->x + c;
      int by = p->y + r;
      if (by < 0 || by >= BOARD_H || bx < 0 || bx >= BOARD_W)
        continue;
      draw_cell(by, bx, cp);
    }
  }
  (void)g;
}

static void draw_next(const Game *g) {
  int px = BOARD_OFF_X + BOARD_W * 2 + 4;
  int py = BOARD_OFF_Y + 2;
  int t = (int)g->next.type;
  int cp = piece_colors[t];

  attron(COLOR_PAIR(CP_UI) | A_BOLD);
  mvprintw(py - 1, px, "NEXT");
  attroff(COLOR_PAIR(CP_UI) | A_BOLD);

  for (int r = 0; r < PIECE_SIZE; r++) {
    int mask = shapes[t][0][r];
    for (int c = 0; c < PIECE_SIZE; c++) {
      if (!(mask & (0x8 >> c)))
        continue;
      attron(COLOR_PAIR(cp) | A_REVERSE);
      mvaddstr(py + r, px + c, "\t");
      attroff(COLOR_PAIR(cp) | A_REVERSE);
    }
  }
}

static void draw_hud(const Game *g) {
  int px = BOARD_OFF_X + BOARD_W * 2 + 4;
  int py = BOARD_OFF_Y + 8;
  attron(COLOR_PAIR(CP_UI) | A_BOLD);
  mvprintw(py, px, "SCORE");
  mvprintw(py + 1, px, "%-7d", g->score);
  mvprintw(py + 3, px, "LEVEL");
  mvprintw(py + 4, px, "%-4d", g->level);
  mvprintw(py + 6, px, "LINES");
  mvprintw(py + 7, px, "%-4d", g->lines);
  mvprintw(py + 10, px, "[<>] move");
  mvprintw(py + 11, px, "[^] rotate");
  mvprintw(py + 12, px, "[v] soft drop");
  mvprintw(py + 13, px, "[spc] hard drop");
  mvprintw(py + 14, px, "[p] pause");
  mvprintw(py + 15, px, "[q] quit");
  attroff(COLOR_PAIR(CP_UI) | A_BOLD);
}

void render_frame(const Game *g) {
  erase();
  draw_border();

  for (int r = 0; r < BOARD_H; r++) {
    for (int c = 0; c < BOARD_W; c++) {
      if (g->board[r][c])
        draw_cell(r, c, g->board[r][c]);
    }
  }

  if (g->running && !g->paused) {
    Piece ghost = compute_ghost(g);
    draw_piece_at(g, &ghost, CP_GHOST);
  }

  if (g->running)
    draw_piece_at(g, &g->current, piece_colors[(int)g->current.type]);

  draw_next(g);
  draw_hud(g);

  if (g->paused) {
    attron(COLOR_PAIR(CP_UI) | A_REVERSE | A_BOLD);
    mvprintw(BOARD_OFF_Y + BOARD_H / 2, BOARD_OFF_X + 2, " PAUSED ");
    attroff(COLOR_PAIR(CP_UI) | A_REVERSE | A_BOLD);
  }

  refresh();
}

void render_gameover(const Game *g) {
  int cx = BOARD_OFF_X + 1;
  int cy = BOARD_OFF_Y + BOARD_H / 2 - 2;
  attron(COLOR_PAIR(5) | A_REVERSE | A_BOLD);
  mvprintw(cy, cx, "                  ");
  mvprintw(cy + 1, cx, "   GAME  OVER!    ");
  mvprintw(cy + 2, cx, "  Score: %-7d  ", g->score);
  mvprintw(cy + 3, cx, "  [r] restart     ");
  mvprintw(cy + 4, cx, "  [q] quit        ");
  mvprintw(cy + 5, cx, "                  ");
  attroff(COLOR_PAIR(5) | A_REVERSE | A_BOLD);
  refresh();
}
