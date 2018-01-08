#pragma once

#include "rltk/base_mode.hpp"
#include "level.hpp"
#include "components/player.hpp"
#include "components/item.hpp"
#include "components/trap.hpp"
#include <vector>
#include <string>
#include <map>

enum input_type_t { NONE, LEFT, RIGHT, UP, DOWN, WAIT, UPLEFT, DOWNLEFT, UPRIGHT, DOWNRIGHT };

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
    void set_door(const int &idx);
    void set_pit_trap(const int &idx, const int &x, const int &y);
    void set_blade_trap(const int &idx, const int &x, const int &y);
    void set_iron_key(const int &idx);
    void set_gold_key(const int &idx);
    bool is_solid(const uint8_t &glyph);

    player_t player;
    item_t amulet;
    std::map<int, trap_t> traps;

    void cast_visibility();
    void render_map(window_t * win);
    void handle_input(window_t * win);
    void do_turn(input_type_t &input);
    void collide(const position_t &pos);

    inline int mapidx(const int &x, const int &y) {
        return (y * level_width) + x;
    }

    std::vector<std::string> log{
        "", "", "", "", ""
    };

    void log_entry(std::string msg);

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
        { SDLK_KP_2, DOWN },

        { SDLK_KP_9, UPRIGHT},
        { SDLK_u, UPRIGHT},

        { SDLK_KP_7, UPLEFT},
        { SDLK_y, UPLEFT},

        { SDLK_KP_1, DOWNLEFT},
        { SDLK_b, DOWNLEFT},

        { SDLK_KP_3, DOWNRIGHT},
        { SDLK_n, DOWNRIGHT},

        { SDLK_PERIOD, WAIT },
        { SDLK_KP_DECIMAL, WAIT }
    };
};
