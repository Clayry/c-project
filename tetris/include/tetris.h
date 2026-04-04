#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define BOARD_W 10
#define BOARD_H 20
#define BOARD_OFF_X 2
#define BOARD_OFF_Y 1

#define NUM_PIECES 7
#define PIECE_SIZE 4

#define SPEED_INIT 600
#define SPEED_MIN 80
#define SPEED_STEP 40

typedef enum {
  PIECE_I = 0,
  PIECE_O,
  PIECE_T,
  PIECE_S,
  PIECE_Z,
  PIECE_J,
  PIECE_L
} PieceType;

typedef struct {
  PieceType type;
  int rot;
  int x, y;
} Piece;

typedef struct {
  int board[BOARD_H][BOARD_W];
  Piece current;
  Piece next;
  int score;
  int lines;
  int level;
  int speed_ms;
  bool running;
  bool paused;
  bool game_over;
} Game;

extern const int shapes[NUM_PIECES][4][4];
extern const int piece_colors[NUM_PIECES];

Piece piece_random(void);
bool piece_fits(const Game *g, const Piece *p);
void piece_lock(Game *g);

void board_init(Game *g);
int board_clear_line(Game *g);

void game_init(Game *g);
void game_update(Game *g);
void game_hard_drop(Game *g);
void game_move(Game *g, int dx);
void game_rotate(Game *g);
void game_soft_drop(Game *g);

void render_init(void);
void render_cleanup(void);
void render_frame(const Game *g);
void render_gameover(const Game *g);

void input_handle(Game *g);

#endif /* ifndef TETRIS_H                                                      \
#DE */
