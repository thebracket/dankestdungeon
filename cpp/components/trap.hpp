#pragma once

#include "position.hpp"

enum trap_type_t { PIT, BLADE };

struct trap_t {
    trap_type_t type = PIT;
    bool revealed = false;
    position_t pos;
};
