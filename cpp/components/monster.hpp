#pragma once

#include "position.hpp"

enum monster_type_t { KOBOLD, ORC, DRAGON };

struct monster_t {
    monster_type_t type;
    bool active = false;
    position_t pos;
    uint8_t glyph;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    int hit_points = 1;
};