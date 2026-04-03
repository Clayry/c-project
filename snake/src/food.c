#include "../include/snake.h"
#include <stdbool.h>
#include <stdlib.h>

static bool on_snake(const Snake *s, Point p) {
  for (int i = 0; i < s->len; i++) {
    if (s->body[i].x == p.x && s->body[i].y == p.y)
      return true;
  }
  return false;
}

void food_spawn(Game *g) {
  Point p;
  do {
    p.x = BOARD_X + rand() % BOARD_W;
    p.y = BOARD_Y + rand() % BOARD_H;
  } while (on_snake(&g->snake, p));
  g->food = p;
}
