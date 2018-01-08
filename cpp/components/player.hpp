#pragma once

#include "position.hpp"

struct player_t {
    position_t pos;
    int hit_points = 10;

    bool has_iron_key = false;
    bool has_gold_key = false;
};
