#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <array>
#include <string>
#include <set>
#include "sprite.hpp"

constexpr int font_size = 16;
constexpr int terminal_width = 80;
constexpr int terminal_height = 40;

class window_t {
public:
    window_t();
    ~window_t();
    void present();
    void poll();

    // Terminal commands
    void cls();
    int loc(const int &x, const int &y);
    void print(const int &x, const int &y, const std::string &msg, int r=255, int g=255, int b=255);
    void print_center(const int &y, const std::string &msg, int r=255, int g=255, int b=255);
    void set(const int &x, const int &y, const uint8_t &glyph, int r=255, int g=255, int b=255);
    void box(const int x, const int y, const int w, const int h, int r=255, int g=255, int b=255, bool double_lines=false);

    // Keyboard input
    bool is_key_down(const int &symbol);
    int mouse_x = 0;
    int mouse_y = 0;
    bool clicked = false;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::unique_ptr<sprite_t> font;

    std::array<uint8_t, terminal_height * terminal_width> console;
    std::array<uint8_t, terminal_height * terminal_width * 3> colors;

    std::set<int> keypress;
};
