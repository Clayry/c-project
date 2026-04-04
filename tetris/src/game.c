#include "../include/tetris.h"
#include <stdbool.h>

static const int LINE_SCORE[5] = {0, 100, 300, 500, 800};

void game_init(Game *g) {
  board_init(g);
  g->score = 0;
  g->lines = 0;
  g->level = 1;
  g->speed_ms = SPEED_INIT;
  g->running = true;
  g->paused = false;
  g->game_over = false;

  g->current = piece_random();
  g->next = piece_random();
}

void game_update(Game *g) {
  if (!g->running || g->paused || g->game_over)
    return;

  Piece moved = g->current;
  moved.y++;

  if (piece_fits(g, &moved)) {
    g->current = moved;
  } else {
    piece_lock(g);

    int cleared = board_clear_line(g);
    if (cleared > 0) {
      g->score += LINE_SCORE[cleared] * g->level;
      g->lines += cleared;

      g->level = g->lines / 10 + 1;

      int speedups = g->lines / 2;
      g->speed_ms = SPEED_INIT - speedups * SPEED_STEP;
      if (g->speed_ms < SPEED_MIN)
        g->speed_ms = SPEED_MIN;
    }

    g->current = g->next;
    g->next = piece_random();

    if (!piece_fits(g, &g->current)) {
      g->game_over = true;
      g->running = false;
    }
  }
}

void game_move(Game *g, int dx) {
  if (!g->running || g->paused)
    return;
  Piece moved = g->current;
  moved.x += dx;
  if (piece_fits(g, &moved))
    g->current = moved;
}

void game_rotate(Game *g) {
  if (!g->running || g->paused)
    return;
  Piece rotated = g->current;
  rotated.rot = (rotated.rot + 1) % 4;

  if (piece_fits(g, &rotated)) {
    g->current = rotated;
    return;
  }
  rotated.x++;
  if (piece_fits(g, &rotated)) {
    g->current = rotated;
    return;
  }
  rotated.x -= 2;
  if (piece_fits(g, &rotated)) {
    g->current = rotated;
    return;
  }
}

void game_soft_drop(Game *g) {
  if (!g->running || g->paused)
    return;
  Piece moved = g->current;
  moved.y++;
  if (piece_fits(g, &moved)) {
    g->current = moved;
    g->score++;
  }
}

void game_hard_drop(Game *g) {
  if (!g->running || g->paused)
    return;
  while (true) {
    Piece moved = g->current;
    moved.y++;
    if (!piece_fits(g, &moved))
      break;
    g->current = moved;
    g->score += 2;
  }
  game_update(g);
}
