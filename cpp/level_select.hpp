#pragma once

#include "rltk/base_mode.hpp"

class level_select_t : public game_mode_t {
    public:
    virtual bool tick(window_t * win);

    private:
    bool first_run = true;
    void get_levels();
    int start = 0;
};
