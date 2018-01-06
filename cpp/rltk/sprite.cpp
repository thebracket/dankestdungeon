#include "sprite.hpp"

sprite_t::sprite_t(SDL_Renderer *renderer, const char * filename) {
    image = IMG_Load(filename);
    if (!image)
    {
        printf("IMG_Load Error: %s\n", IMG_GetError());
    } else {
        tex = SDL_CreateTextureFromSurface(renderer, image);
    }
}

void sprite_t::display(SDL_Renderer *renderer) {
    SDL_Rect dest = {.x = 200, .y = 100, .w = 200, .h = 200};
    SDL_RenderCopy (renderer, tex, nullptr, &dest);
}

void sprite_t::render_subsprite(SDL_Renderer *renderer, const int &width, const int &height, const int &x, const int &y, const int &src_x, const int &src_y, uint8_t red, uint8_t green, uint8_t blue) {
    SDL_Rect dest = {.x = x, .y = y, .w = width, .h = height};
    SDL_Rect src = {.x = src_x, .y = src_y, .w = width, .h = height};
    SDL_SetTextureColorMod(tex, red, green, blue);
    SDL_RenderCopy (renderer, tex, &src, &dest);
}

sprite_t::~sprite_t() {
    SDL_DestroyTexture (tex);
    SDL_FreeSurface (image);
}
