#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdlib.h>
#include <memory.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define FPS 60

#define MAX_STARS 500

#define MAX_KEYBOARD_SIZE 350

#define SIDE_PLAYER 0
#define SIDE_ENEMY  1

static const int PLAYER_SPEED        = 4;
static const int PLAYER_RELOAD       = 8;
static const int BULLET_ACCELERATION = 16;

static const int ENEMY_BULLET_SPEED  = 8;

static SDL_Texture *player_texture;
static SDL_Texture *bullet_texture;

static SDL_Texture *enemy_texture;
static SDL_Texture *enemy_bullet_texture;

static SDL_Texture *background;
static SDL_Texture *explosion_texture;

static int enemy_spawn_timer;
static int stage_reset_timer;
static int backgroundX;

SDL_Texture *load_texture(char *file_name);
static void blit(SDL_Texture *texture, int x, int y);
static void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y);

inline static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
inline static void calc_slope(int x1, int y1, int x2, int y2, float *dx, float *dy);

#endif
