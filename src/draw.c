#include "common.h"
#include "structs.h"

void prepare_scene(void)
{
    SDL_SetRenderDrawColor(app.renderer, 32, 32, 32, 255);
    SDL_RenderClear(app.renderer);
}

void present_scene(void)
{
    SDL_RenderPresent(app.renderer);
}

SDL_Texture *load_texture(char *file_name)
{
    SDL_Texture *texture;
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", file_name);
    texture = IMG_LoadTexture(app.renderer, file_name);
    return texture;
}

void blit(SDL_Texture *texture, int x, int y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}
