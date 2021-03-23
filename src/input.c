#include "common.h"
#include "structs.h"

void do_keydown(SDL_KeyboardEvent *event)
{
    if(event->repeat == 0)
    {
        if(event->keysym.scancode < MAX_KEYBOARD_SIZE)
            app.keyboard[event->keysym.scancode] = 1;
    }
    else
    {
        // do nothing.
    }
}

void do_keyup(SDL_KeyboardEvent *event)
{
    if(event->repeat == 0)
    {
        if(event->keysym.scancode < MAX_KEYBOARD_SIZE)
            app.keyboard[event->keysym.scancode] = 0;
    }
    else
    {
        // do nothing.
    }
}

void do_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:    exit(0); break;

            case SDL_KEYDOWN:do_keydown(&event.key);break;
            case SDL_KEYUP:  do_keyup(&event.key);break;

            default: break;
        }
    }

    // WASD controls
    controller.right = app.keyboard[SDL_SCANCODE_D];
    controller.left  = app.keyboard[SDL_SCANCODE_A];
    controller.up    = app.keyboard[SDL_SCANCODE_W];
    controller.down  = app.keyboard[SDL_SCANCODE_S];
    controller.fire  = app.keyboard[SDL_SCANCODE_SPACE];
}
