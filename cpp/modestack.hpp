#pragma once

#include "rltk/base_mode.hpp"
#include <memory>
#include <stack>

enum return_state_t { IN_PROGRESS, WON, DEAD };

extern std::stack<std::unique_ptr<game_mode_t>> modestack;
extern return_state_t game_exit_state;
extern int game_exit_turns;
