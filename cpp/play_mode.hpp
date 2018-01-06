#pragma once

#include "rltk/base_mode.hpp"

class play_mode_t : public game_mode_t {
    public:
    virtual bool tick(window_t * win);
};
