#include "play_mode.hpp"

bool play_mode_t::tick(window_t * win) {
    win->cls();
    win->print_center(1, "Play Mode", 255, 255, 0);
    win->print_center(2, "ESC to return to main menu", 128, 128, 128);

    if (win->is_key_down(SDLK_ESCAPE)) {
        return true;
    }
    return false;
}
