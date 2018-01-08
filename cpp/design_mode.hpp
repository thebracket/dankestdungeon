#pragma once

#include "rltk/base_mode.hpp"
#include "level.hpp"
#include <vector>

enum design_mode_e { TERRAIN, SPECIAL, PUZZLE, MOB, TRAP, PERKS };

struct design_element_t {
    uint8_t glyph;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    std::string title;
    int hotkey;
};

class design_mode_t : public game_mode_t {
    public:
    virtual bool tick(window_t * win);

    private:
    bool first_run = true;
    level_t level;
    design_mode_e mode = TERRAIN;
    uint8_t paint_glyph = '.';
    uint8_t paint_r = 255;
    uint8_t paint_g = 255;
    uint8_t paint_b = 255;

    void initialize_dungeon();
    void render_map(window_t * win);
    void render_ui(window_t * win);
    void handle_inputs(window_t * win);

    void render_submenu(window_t * win, std::vector<design_element_t> * elements);

    int cx = level_width / 2;
    int cy = level_height / 2;

    std::vector<design_element_t> terrain_elements = {
        design_element_t{'.', 128, 128, 128, "(F)loor", SDLK_f},
        design_element_t{176, 128, 128, 128, "(W)all", SDLK_w},
        design_element_t{6, 230, 10, 238, "(T)oadstool", SDLK_t},
        design_element_t{5, 0, 255, 0, "(V)egetation", SDLK_p},
        design_element_t{'+', 128, 128, 128, "(D)oor - normal", SDLK_d},
        design_element_t{'~', 0, 0, 255, "(P)Pool", SDLK_p}
    };
    std::vector<design_element_t> special_elements = {
        design_element_t{'@', 255, 255, 0, "(P)layer", SDLK_p},
        design_element_t{157, 0, 255, 0, "(A)mulet of Winning", SDLK_a}
    };
    std::vector<design_element_t> puzzle_elements = {
        design_element_t{'!', 255, 255, 255, "(I)ron Door, Locked", SDLK_i},
        design_element_t{236, 255, 255, 255, "(O)Iron Key", SDLK_o},
        design_element_t{240, 255, 255, 0, "(G)old Door, Locked", SDLK_g},
        design_element_t{237, 255, 255, 0, "(H)Gold Key", SDLK_h},
    };
    std::vector<design_element_t> mob_elements = {
        design_element_t{'k', 255, 0, 0, "(K)obold", SDLK_k},
        design_element_t{'o', 255, 0, 0, "(O)rc", SDLK_o},
        design_element_t{'D', 255, 128, 128, "(D)ragon", SDLK_d}
    };
    std::vector<design_element_t> trap_elements = {
        design_element_t{127, 255, 255, 0, "(P)it trap", SDLK_p},
        design_element_t{241, 0, 255, 0, "(B)lade trap", SDLK_b}
    };
    std::vector<design_element_t> perk_elements = {
        design_element_t{205, 0, 255, 0, "(M)apping Scroll", SDLK_m},
        design_element_t{229, 0, 255, 0, "(H)ealing Potion", SDLK_h}
    };
};
