#pragma once

#include "rltk/base_mode.hpp"
#include <array>
#include <string>

struct menu_item_t {
    std::string title;
    int retcode;
    int hotkey;
};

class main_menu_t : public game_mode_t {
    public:
    virtual bool tick(window_t * win);

    private:
    int selection = 0;
    std::array<menu_item_t, 3> menu_options = {
        menu_item_t{"(1) - Create a Dungeon Challenge", 1, SDLK_1}, 
        menu_item_t{"(2) - Play a Dungeon Challenge", 2, SDLK_2},
    };

    void launch_item(const int code);
};
