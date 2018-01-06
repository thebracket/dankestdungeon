#pragma once

#include "window.hpp"

class game_mode_t {
    public:
    virtual bool tick(window_t * mainWindow)=0;
};

