#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class sprite_t {
public:
    sprite_t(SDL_Renderer *renderer, const char * filename);
    ~sprite_t();
    void display(SDL_Renderer *renderer);
    void render_subsprite(SDL_Renderer *renderer, const int &width, const int &height, const int &x, const int &y, const int &src_x, const int &src_y, uint8_t red=255, uint8_t green=255, uint8_t blue=255);

private:
    SDL_Texture *tex = nullptr;
    SDL_Surface *image = nullptr;
};
