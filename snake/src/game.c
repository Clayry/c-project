#include "../include/snake.h"
#include <stdbool.h>
#include <string.h>

void game_init(Game *g) {
  memset(g, 0, sizeof(*g));

  g->snake.len = 3;
  g->snake.dir = RIGHT;
  g->snake.next_dir = RIGHT;

  int cx = BOARD_X + BOARD_W / 2;
  int cy = BOARD_Y + BOARD_H / 2;
  for (int i = 0; i < g->snake.len; i++) {
    g->snake.body[i].x = cx - i;
    g->snake.body[i].y = cy;
  }

  g->score = 0;
  g->speed_ms = SPEED_INIT;
  g->running = true;
  g->paused = false;

  food_spawn(g);
}

static bool in_bounds(Point p) {
  return p.x >= BOARD_X && p.x < BOARD_X + BOARD_W && p.y >= BOARD_Y &&
         p.y < BOARD_Y + BOARD_H;
}

static bool hits_self(const Snake *s) {
  for (int i = 1; i < s->len; i++) {
    if (s->body[0].x == s->body[i].x && s->body[0].y == s->body[i].y)
      return true;
  }
  return false;
}

void game_update(Game *g) {
  if (!g->running || g->paused)
    return;

  Snake *s = &g->snake;

  Dir nd = s->next_dir;
  if (!((s->dir == UP && nd == DOWN) || (s->dir == DOWN && nd == UP) ||
        (s->dir == LEFT && nd == RIGHT) || (s->dir == RIGHT && nd == LEFT)))
    s->dir = nd;

  for (int i = s->len - 1; i > 0; i--) {
    s->body[i] = s->body[i - 1];
  }

  switch (s->dir) {
  case UP:
    s->body[0].y--;
    break;
  case DOWN:
    s->body[0].y++;
    break;
  case LEFT:
    s->body[0].x--;
    break;
  case RIGHT:
    s->body[0].x++;
    break;
  }

  if (!in_bounds(s->body[0]) || hits_self(s)) {
    g->running = false;
    return;
  }

  if (s->body[0].x == g->food.x && s->body[0].y == g->food.y) {
    g->score++;
    if (s->len < MAX_LEN)
      s->len++;

    if (g->score % 5 == 0 && g->speed_ms > SPEED_MIN)
      g->speed_ms -= SPEED_STEP;

    food_spawn(g);
  }
}
