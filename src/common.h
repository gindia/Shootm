#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdlib.h>
#include <memory.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720


#define MAX_KEYBOARD_SIZE 350

#define SIDE_PLAYER 0
#define SIDE_ENEMY  0

static const int PLAYER_SPEED        = 4;
static const int PLAYER_RELOAD       = 8;
static const int BULLET_ACCELERATION = 16;

static SDL_Texture *bullet_texture;
static SDL_Texture *enemy_texture;

static int enemy_spawn_timer;

SDL_Texture *load_texture(char *file_name);
void blit(SDL_Texture *texture, int x, int y);

static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
#endif
