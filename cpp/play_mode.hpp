#pragma once

#include "rltk/base_mode.hpp"
#include "level.hpp"
#include "components/player.hpp"
#include <vector>

enum input_type_t { NONE, LEFT, RIGHT, UP, DOWN };

struct input_map_t {
    int code;
    input_type_t input_type;
};

class play_mode_t : public game_mode_t {
public:
    play_mode_t() {}
    play_mode_t(const level_t * level);
    virtual bool tick(window_t * win);
private:
    bool playtesting = false;

    int turn = 0;

    std::array<tile_t, level_tiles> tile_render;
    std::array<bool, level_tiles> tile_solid;
    std::array<bool, level_tiles> tile_revealed;
    std::array<bool, level_tiles> tile_visible;

    void set_floor(const int &idx);
    bool is_solid(const uint8_t &glyph);

    player_t player;

    void cast_visibility();
    void render_map(window_t * win);
    void handle_input(window_t * win);
    void do_turn(input_type_t &input);

    inline int mapidx(const int &x, const int &y) {
        return (y * level_width) + x;
    }

    /* Quick input mapper. In a real app, this would be adjustable! */
    std::vector<input_map_t> input_mapper {
        { SDLK_LEFT, LEFT },
        { SDLK_h, LEFT },
        { SDLK_KP_4, LEFT },

        { SDLK_RIGHT, RIGHT },
        { SDLK_l, RIGHT },
        { SDLK_KP_6, RIGHT },

        { SDLK_UP, UP },
        { SDLK_j, UP },
        { SDLK_KP_8, UP },

        { SDLK_DOWN, DOWN },
        { SDLK_k, DOWN },
        { SDLK_KP_2, DOWN }
    };
};
