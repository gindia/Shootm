#ifndef STRUCTS_H
#define STRUCTS_H

#include "common.h"

typedef struct {
    void (*logic)(void);
    void (*draw)(void);
} Delegate;

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window   *window;
    Delegate delegate;
    int keyboard[MAX_KEYBOARD_SIZE];
} App;

typedef struct{
    int up;
    int down;
    int left;
    int right;
    int fire;
} Controller;

typedef struct Entity Entity;
struct Entity{
    int x;
    int y;
    int w;
    int h;
    float dx;
    float dy;
    int health;
    int reload;
    int side;
    SDL_Texture *texture;
    Entity *next;
};

typedef struct {
    Entity fighterHead, *fighterTail;
    Entity bulletHead,  *bulletTail;
} Stage;

static App        app;
static Controller controller;
static Stage      stage;
static Entity     *player;
#endif
