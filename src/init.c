#include "common.h"
#include "structs.h"

void init_sdl(void)
{
    int render_flags, window_flags;

    render_flags = SDL_RENDERER_ACCELERATED;
    window_flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_GRABBED;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "init sdl", SDL_GetError(), NULL);
        exit(1);
    }
    else
    {
        app.window = SDL_CreateWindow("Shootm",
                                       350, 200,
                                       SCREEN_WIDTH, SCREEN_HEIGHT,
                                       window_flags);
        if(app.window == NULL)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "init sdl", SDL_GetError(), NULL);
            exit(1);
        }
        else
        {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
            app.renderer = SDL_CreateRenderer(app.window, -1, render_flags);
            if(app.renderer == NULL)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "init sdl", SDL_GetError(), NULL);
                exit(1);
            }
            else
            {
                if( IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0 )
                {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "init sdl_img", SDL_GetError(), NULL);
                    exit(1);
                }
                else
                {
                    // succeeded
                }
            }
        }
    }
}

void cleanup(void)
{
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    //free(player);
}
