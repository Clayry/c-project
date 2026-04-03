#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h>
#include <stdbool.h>

#define BOARD_W 40
#define BOARD_H 20
#define BOARD_X 2
#define BOARD_Y 2

#define MAX_LEN (BOARD_W * BOARD_H)

#define SPEED_INIT 150
#define SPEED_MIN 50
#define SPEED_STEP 10

typedef enum { UP, DOWN, LEFT, RIGHT } Dir;

typedef struct {
  int x, y;
} Point;

typedef struct {
  Point body[MAX_LEN];
  int len;
  Dir dir;
  Dir next_dir;
} Snake;

typedef struct {
  Snake snake;
  Point food;
  int score;
  int speed_ms;
  bool running;
  bool paused;
} Game;

void game_init(Game *g);
void game_update(Game *g);
bool game_over_check(const Game *g);

void food_spawn(Game *g);

void render_init(void);
void render_cleanup(void);
void render_frame(const Game *g);
void render_gameover(const Game *g);

void input_handle(Game *g);

#endif
