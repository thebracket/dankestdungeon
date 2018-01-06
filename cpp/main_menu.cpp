#include "main_menu.hpp"
#include "leaderboards.hpp"
#include "design_mode.hpp"
#include "play_mode.hpp"
#include "modestack.hpp"

bool main_menu_t::tick(window_t * win) {
    const std::string title = std::string("Welcome to Dankest Dungeon");
    const int bx = (terminal_width/2 - title.size()/2)-1;

    win->cls();
    win->box(bx, 4, title.size()+1, 4, 255, 255, 255, true);
    win->print_center(5, "Welcome to Dankest Dungeon", 255, 255, 0);
    win->print_center(7, "A 7-Day Bracket Production", 255, 0, 255);
    win->print_center(terminal_height-2, "(c) 2018 Bracket Productions", 0, 0, 255);

    for (int i=0; i<menu_options.size(); ++i) {
        if (win->mouse_y == 11+i) {
            selection = i;
            if (win->clicked) launch_item(menu_options[i].retcode);
        }

        if (selection == i) {
            win->print_center(11+i, menu_options[i].title, 255, 255, 255);
            if (win->is_key_down(SDLK_RETURN)) {
                launch_item(menu_options[i].retcode);
            }
        } else {
            win->print_center(11+i, menu_options[i].title, 255, 0, 0);
        }
        if (win->is_key_down(menu_options[i].hotkey)) {
            launch_item(menu_options[i].retcode);
        }
    }

    if (win->is_key_down(SDLK_UP)) {
        --selection;
        if (selection < 0) selection = menu_options.size()-1;
    }
    if (win->is_key_down(SDLK_DOWN)) {
        ++selection;
        if (selection > menu_options.size()-1) selection = 0;
    }

    return false;
}

void main_menu_t::launch_item(const int code) {
    switch (code) {
        case 1 : {
            modestack.emplace(std::make_unique<design_mode_t>());
        } break;
        case 2 : {
            modestack.emplace(std::make_unique<play_mode_t>());
        } break;
        case 3 : {
            modestack.emplace(std::make_unique<leaderboards_t>());
        } break;
    }
}