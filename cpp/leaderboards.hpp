#pragma once

#include "rltk/base_mode.hpp"

class leaderboards_t : public game_mode_t {
    public:
    virtual bool tick(window_t * win);
};
