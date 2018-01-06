#pragma once

#include "rltk/base_mode.hpp"
#include <memory>
#include <stack>

extern std::stack<std::unique_ptr<game_mode_t>> modestack;
