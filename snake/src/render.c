#include "../include/snake.h"
#include <ncurses.h>
#include <stdio.h>

#define CP_BORDER 1
#define CP_SNAKE 2
#define CP_HEAD 3
#define CP_FOOD 4
#define CP_SCORE 5
#define CP_DEAD 6
#define CP_PAUSED 7

void render_init(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);

  if (has_colors()) {
    start_color();
    init_pair(CP_BORDER, COLOR_CYAN, COLOR_BLACK);
    init_pair(CP_SNAKE, COLOR_GREEN, COLOR_BLACK);
    init_pair(CP_HEAD, COLOR_WHITE, COLOR_GREEN);
    init_pair(CP_FOOD, COLOR_RED, COLOR_BLACK);
    init_pair(CP_SCORE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CP_DEAD, COLOR_WHITE, COLOR_RED);
    init_pair(CP_PAUSED, COLOR_BLACK, COLOR_YELLOW);
  }
}

void render_cleanup(void) { endwin(); }

static void draw_border(void) {
  attron(COLOR_PAIR(CP_BORDER) | A_BOLD);

  for (int x = BOARD_X - 1; x <= BOARD_X + BOARD_W; x++) {
    mvaddch(BOARD_Y - 1, x, ACS_HLINE);
    mvaddch(BOARD_Y + BOARD_H, x, ACS_HLINE);
  }

  for (int y = BOARD_Y - 1; y <= BOARD_Y + BOARD_H; y++) {
    mvaddch(y, BOARD_X - 1, ACS_VLINE);
    mvaddch(y, BOARD_X + BOARD_W, ACS_VLINE);
  }

  mvaddch(BOARD_Y - 1, BOARD_X - 1, ACS_ULCORNER);
  mvaddch(BOARD_Y - 1, BOARD_X + BOARD_W, ACS_URCORNER);
  mvaddch(BOARD_Y + BOARD_H, BOARD_X - 1, ACS_LLCORNER);
  mvaddch(BOARD_Y + BOARD_H, BOARD_X + BOARD_W, ACS_LRCORNER);

  attroff(COLOR_PAIR(CP_BORDER) | A_BOLD);
}

void render_frame(const Game *g) {
  erase();
  draw_border();

  attron(COLOR_PAIR(CP_SCORE) | A_BOLD);
  mvprintw(0, BOARD_X, " SNAKE Score: %-4d Speed; %dms ", g->score,
           g->speed_ms);
  mvprintw(0, BOARD_X + BOARD_W - 22, " [arrows] move [p] pause ");
  attroff(COLOR_PAIR(CP_SCORE) | A_BOLD);

  attron(COLOR_PAIR(CP_FOOD) | A_BOLD);
  mvaddch(g->food.y, g->food.x, ACS_DIAMOND);
  attroff(COLOR_PAIR(CP_FOOD) | A_BOLD);

  const Snake *s = &g->snake;
  for (int i = 1; i < s->len; i++) {
    attron(COLOR_PAIR(CP_SNAKE));
    mvaddch(s->body[i].y, s->body[i].x, ACS_CKBOARD);
    attroff(COLOR_PAIR(CP_SNAKE));
  }

  attron(COLOR_PAIR(CP_HEAD) | A_BOLD);
  char head_ch = (s->dir == UP || s->dir == DOWN) ? 'O' : 'O';
  mvaddch(s->body[0].y, s->body[0].x, head_ch);
  attroff(COLOR_PAIR(CP_HEAD) | A_BOLD);

  if (g->paused) {
    int mid_x = BOARD_X + BOARD_W / 2 - 4;
    int mid_y = BOARD_Y + BOARD_H / 2;
    attron(COLOR_PAIR(CP_PAUSED) | A_BOLD);
    mvprintw(mid_y, mid_x, " PAUSED ");
    attroff(COLOR_PAIR(CP_PAUSED) | A_BOLD);
  }

  refresh();
}

void render_gameover(const Game *g) {
  int mid_x = BOARD_X + BOARD_W / 2 - 10;
  int mid_y = BOARD_Y + BOARD_H / 2 - 1;

  attron(COLOR_PAIR(CP_DEAD) | A_BOLD);
  mvprintw(mid_y, mid_x, "                      ");
  mvprintw(mid_y + 1, mid_x, "    GAME OVER  :(     ");
  mvprintw(mid_y + 2, mid_x, "  Score: %-4d         ", g->score);
  mvprintw(mid_y + 3, mid_x, "  [r] restart  [q] quit ");
  mvprintw(mid_y + 4, mid_x, "                      ");
  attroff(COLOR_PAIR(CP_DEAD) | A_BOLD);

  refresh();
}
