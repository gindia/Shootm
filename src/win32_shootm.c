#include "common.h"
#include "structs.h"

#include "init.c"
#include "draw.c"
#include "input.c"
#include "stage.c"
#include "util.c"

#include "Windows.h"

static void cap_frame_rate(long *then ,float *remainder);

int WINAPI wWinMain (HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     PWSTR pCmdLine,
                     int nCmdShow)
{
    SDL_SetMainReady();

    long then;
    float remainder;

    memset(&app, 0, sizeof(App));

    init_sdl();

    atexit(cleanup);

    init_stage();

    then = SDL_GetTicks();
    remainder = 0.f;

    while(1)
    {
        prepare_scene();

        do_input();

        /* exit shortcut */
        if (app.keyboard[SDL_SCANCODE_Q] == 1)
            break;

        //app.delegate.logic();
        //app.delegate.draw();
        logic();
        draw();


        present_scene();

        cap_frame_rate(&then ,&remainder);
    }

    return 0;
}

static void cap_frame_rate(long *then ,float *remainder)
{
    long wait, frame_time;

    wait = 16 + *remainder;

    *remainder -= (int)*remainder;

    frame_time = SDL_GetTicks() - *then;

    wait -= frame_time;

    if (wait < 1)
    {
        wait = 1;
    }

    SDL_Delay(wait);

    *remainder += 0.667;

    *then = SDL_GetTicks();
}
