#include "modestack.hpp"

std::stack<std::unique_ptr<game_mode_t>> modestack;
return_state_t game_exit_state = IN_PROGRESS;
int game_exit_turns = 0;
std::string username;
int token;
