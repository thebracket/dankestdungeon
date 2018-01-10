#include "play_mode.hpp"
#include "rltk/visibility.hpp"
#include "components/position.hpp"
#include <algorithm>
#include <vector>
#include "modestack.hpp"
#include <emscripten.h>
#include <emscripten/fetch.h>

play_mode_t::play_mode_t(const level_t * level, bool testing) {
    // Playtest constructor - copy the level to the playable format.
    playtesting = testing;
    game_exit_state = IN_PROGRESS;
    game_exit_turns = 0;

    if (!testing) {
        levelId = level->id;
        levelName = level->name;
        levelCreator = level->creator;
    }

    int x = 0;
    int y = 0;
    for (std::size_t i=0; i < level_tiles; ++i) {
        tile_render[i] = level->tiles[i];
        
        // Replace player with floor and note position
        if (tile_render[i].glyph == '@') {
            set_floor(i);
            player.pos.x = x;
            player.pos.y = y;
        } else if (tile_render[i].glyph == 157) {
            set_floor(i);
            amulet.name = "Amulet of Winning";
            amulet.pos.x = x;
            amulet.pos.y = y;
        } else if (tile_render[i].glyph == '+') {
            set_door(i); // Doors
        } else if (tile_render[i].glyph == 236) {
            set_iron_key(i);
        } else if (tile_render[i].glyph == '!') {
            // TODO: Iron Door
        } else if (tile_render[i].glyph == 237) {
            set_gold_key(i);
        } else if (tile_render[i].glyph == 240) {
            // TODO: Gold Door
        } else if (tile_render[i].glyph == 'k') {
            set_monster(KOBOLD, i, x, y);
        } else if (tile_render[i].glyph == 'o') {
            set_monster(ORC, i, x, y);
        } else if (tile_render[i].glyph == 'D') {
            set_monster(DRAGON, i, x, y);
        } else if (tile_render[i].glyph == 127) {
            set_pit_trap(i, x, y);
        } else if (tile_render[i].glyph == 241) {
            set_blade_trap(i, x, y);
        } else if (tile_render[i].glyph == 205) {
            tile_render[i].glyph = 205;
            tile_render[i].r = 0;
            tile_render[i].g = 255;
            tile_render[i].b = 0;
        } else if (tile_render[i].glyph == 229) {
            tile_render[i].glyph = 229;
            tile_render[i].r = 0;
            tile_render[i].g = 255;
            tile_render[i].b = 0;
        }

        // Other info
        tile_visible[i] = false;
        tile_revealed[i] = false;
        tile_solid[i] = is_solid(tile_render[i].glyph);

        ++x;
        if (x >= level_width) {
            x = 0;
            ++y;
        }
    }

    cast_visibility();
    log_entry("Welcome to the dungeon, we've got fun and games...");
}

void play_mode_t::set_floor(const int &idx) {
    tile_render[idx].glyph = '.';
    tile_render[idx].r = 128;
    tile_render[idx].g = 128;
    tile_render[idx].b = 128;
}

void play_mode_t::set_pit_trap(const int &idx, const int &x, const int &y) {
    tile_render[idx].glyph = '.';
    tile_render[idx].r = 128;
    tile_render[idx].g = 128;
    tile_render[idx].b = 128;
    trap_t newtrap{ PIT, false, position_t{static_cast<uint8_t>(x), static_cast<uint8_t>(y)}};
    traps.insert(std::make_pair(idx, newtrap));
}

void play_mode_t::set_blade_trap(const int &idx, const int &x, const int &y) {
    tile_render[idx].glyph = '.';
    tile_render[idx].r = 128;
    tile_render[idx].g = 128;
    tile_render[idx].b = 128;
    trap_t newtrap{ BLADE, false, position_t{static_cast<uint8_t>(x), static_cast<uint8_t>(y)}};
    traps.insert(std::make_pair(idx, newtrap));
}

void play_mode_t::set_door(const int &idx) {
    tile_render[idx].glyph = '+';
    tile_render[idx].r = 128;
    tile_render[idx].g = 128;
    tile_render[idx].b = 128;
    tile_solid[idx] = true;
}

void play_mode_t::set_iron_key(const int &idx) {
    tile_render[idx].glyph = 236;
    tile_render[idx].r = 255;
    tile_render[idx].g = 255;
    tile_render[idx].b = 255;
}

void play_mode_t::set_gold_key(const int &idx) {
    tile_render[idx].glyph = 237;
    tile_render[idx].r = 255;
    tile_render[idx].g = 255;
    tile_render[idx].b = 0;
}

bool play_mode_t::is_solid(const uint8_t &glyph) {
    switch (glyph) {
        case '.': return false;
        case 236 : return false;
        case 237 : return false;
        case 229 : return false;
        case 205 : return false;
        case '~': return true;
        default: return true;
    }
}

void play_mode_t::set_monster(const monster_type_t &type, const int &idx, const int &x, const int &y) {
    tile_render[idx].glyph = '.';
    tile_render[idx].r = 128;
    tile_render[idx].g = 128;
    tile_render[idx].b = 128;

    uint8_t glyph=0, r=255, g=0, b=0;
    int hp = 0;
    switch (type) {
        case KOBOLD : { glyph='k'; hp = 1; } break;
        case ORC : { glyph='o'; hp = 3; } break;
        case DRAGON : { glyph='D'; hp = 20; } break;
        default: printf("Unknown monster type!\n");
    }

    monster_t newmonster{ type, false, position_t{static_cast<uint8_t>(x), static_cast<uint8_t>(y)}, glyph, r, g, b, hp };
    monsters.insert(std::make_pair(idx, newmonster));
}

void play_mode_t::cast_visibility() {
    std::fill(tile_visible.begin(), tile_visible.end(), false);
    rltk::visibility_sweep_2d<position_t, navigator_t>(player.pos, 16, 
        [this] (position_t pos) {
            if (pos.x > 0 && pos.x < level_width && pos.y > 0 && pos.y < level_height) {
                const int idx = this->mapidx(pos.x, pos.y);
                this->tile_revealed[idx] = true; 
                this->tile_visible[idx] = true;
            }
            }, 
        [this] (position_t pos) { 
            if (pos.x > 0 && pos.x < level_width && pos.y > 0 && pos.y < level_height) {
                return !tile_solid[this->mapidx(pos.x, pos.y)]; 
            } else {
                return true;
            }
        }
    );
}

bool play_mode_t::tick(window_t * win) {
    render_map(win);
    handle_input(win);

    // Temporary: DELETEME when exit conditions work.
    if (playtesting && win->is_key_down(SDLK_ESCAPE)) {
        return true;
    }
        // Check for win/loss conditions
    if (player.pos.x == amulet.pos.x && player.pos.y == amulet.pos.y) {
        printf("Player was won the game in %d turns.\n", turn);
        game_exit_state = WON;
        game_exit_turns = turn;

        if (!playtesting) {
            EM_ASM(
                winGame($0, $1);
            , game_exit_turns, levelId);
        }

        return true;
    }
    if (player.hit_points < 1) {
        printf("Player is dead\n");
        game_exit_state = DEAD;
        game_exit_turns = turn;

        EM_ASM(
            loseGame($0, $1);
            , game_exit_turns, levelId);

        return true;
    }

    return false;
}

void play_mode_t::handle_input(window_t * win) {
    input_type_t input_type = NONE;
    for (const auto &im : input_mapper) {
        if (win->is_key_down(im.code)) input_type = im.input_type;
    }

    if (input_type != NONE) {
        do_turn(input_type);
    }
}

bool play_mode_t::check_attack(const int &x, const int &y) {
    std::vector<int> to_erase;
    bool retval = false;

    for (auto &mob : monsters) {
        if (mob.second.pos.x == x && mob.second.pos.y == y) {
            if (mob.second.type != DRAGON) mob.second.hit_points -= 5;
            if (mob.second.hit_points < 1) {
                to_erase.push_back(mob.first);
                switch (mob.second.type) {
                    case KOBOLD : log_entry("You bash the kobold on the head, killing it."); break;
                    case ORC : log_entry("With a mightly swing, you behead the orc."); break;
                    case DRAGON : log_entry("You tickle the dragon with your sword. It doesn't like it."); break;
                }
            }

            retval = true;
        }
    }

    for (const auto &idx : to_erase) {
        monsters.erase(idx);
    }

    return retval;
}

void play_mode_t::do_turn(input_type_t &input) {
    // Player
    bool player_moved = false;    
    if (input == LEFT && player.pos.x > 1) {
        if (!tile_solid[mapidx(player.pos.x - 1, player.pos.y)]) {
            if (!check_attack(player.pos.x - 1, player.pos.y)) {
                --player.pos.x;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            --dest.x;
            collide(dest);
        }
    } else if (input == RIGHT && player.pos.x < level_width - 1) {
        if (!tile_solid[mapidx(player.pos.x + 1, player.pos.y)]) {
            if (!check_attack(player.pos.x + 1, player.pos.y)) {
                ++player.pos.x;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            ++dest.x;
            collide(dest);
        }
    } else if (input == UP && player.pos.y > 1) {
        if (!tile_solid[mapidx(player.pos.x, player.pos.y-1)]) {
            if (!check_attack(player.pos.x, player.pos.y-1)) {
                --player.pos.y;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            --dest.y;
            collide(dest);
        }
    } else if (input == DOWN && player.pos.y < level_height - 1) {
        if (!tile_solid[mapidx(player.pos.x, player.pos.y+1)]) {
            if (!check_attack(player.pos.x, player.pos.y+1)) {
                ++player.pos.y;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            ++dest.y;
            collide(dest);
        }
    } else if (input == UPLEFT && player.pos.x > 1 && player.pos.y > 1) {
        if (!tile_solid[mapidx(player.pos.x-1, player.pos.y-1)]) {
            if (!check_attack(player.pos.x - 1, player.pos.y - 1)) {
                --player.pos.y;
                --player.pos.x;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            --dest.y;
            --dest.x;
            collide(dest);
        }
    } else if (input == UPRIGHT && player.pos.x < level_width-1 && player.pos.y > 1) {
        if (!tile_solid[mapidx(player.pos.x+1, player.pos.y-1)]) {
            if (!check_attack(player.pos.x+1, player.pos.y-1)) {
                --player.pos.y;
                ++player.pos.x;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            --dest.y;
            ++dest.x;
            collide(dest);
        }
    } else if (input == DOWNLEFT && player.pos.x > 1 && player.pos.y < level_height-1) {
        if (!tile_solid[mapidx(player.pos.x-1, player.pos.y+1)]) {
            if (!check_attack(player.pos.x - 1, player.pos.y+1)) {
                ++player.pos.y;
                --player.pos.x;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            ++dest.y;
            --dest.x;
            collide(dest);
        }
    } else if (input == DOWNRIGHT && player.pos.x < level_width-1 && player.pos.y < level_height-1) {
        if (!tile_solid[mapidx(player.pos.x+1, player.pos.y+1)]) {
            if (!check_attack(player.pos.x+1, player.pos.y+1)) {
                ++player.pos.y;
                ++player.pos.x;
                player_moved = true;
            }
        } else {
            position_t dest = player.pos;
            ++dest.y;
            ++dest.x;
            collide(dest);
        }
    } else if (input == WAIT) {
        for (auto &trap : traps) {
            if (tile_visible[mapidx(trap.second.pos.x, trap.second.pos.y)]) {
                trap.second.revealed = true;
                log_entry("You spotted a trap!");
            }
        }
    }

    // Redo visibility after movement
    if (player_moved) {
        cast_visibility();
        const int idx = mapidx(player.pos.x, player.pos.y);
        auto finder = traps.find(idx);
        if (finder != traps.end()) {
            player.hit_points -= 3;
            switch (finder->second.type) {
                case PIT : log_entry("You stumble into a pit trap, taking 3 points of damage."); break;
                case BLADE : log_entry("Snickety snick! Blades shoot out of the floor, hitting you for 3 points of damage."); break;
            }
        }
    }

    // Update counters
    ++turn;

    // Monsters
    for (auto &mob : monsters) {
        // Set to inactive
        mob.second.active = false;

        // If in range, can it see the player?
        const float mob_range = rltk::distance2d(player.pos.x, player.pos.y, mob.second.pos.x, mob.second.pos.y);
        if (mob_range < 10.0f) {
            rltk::visibility_sweep_2d<position_t, navigator_t>(mob.second.pos, 8, [this, &mob] (position_t pos) {
                // Hit tile
                if (pos.x == this->player.pos.x && pos.y == this->player.pos.y) {
                    mob.second.active = true;
                }
            }, [this] (position_t pos) {
                // Is solid
                if (pos.x > 0 && pos.x < level_width && pos.y > 0 && pos.y < level_height) {
                    return !tile_solid[this->mapidx(pos.x, pos.y)]; 
                } else {
                    return true;
                }
            });
        }

        // Are we active?
        if (mob.second.active) {
            if (mob_range < 1.5f) {
                // Adjacent - attack!
                switch (mob.second.type) {
                    case KOBOLD : { 
                        log_entry("The kobold pokes you with a sharp stick (1 HP damage).");
                        --player.hit_points;
                    } break;
                    case ORC : { 
                        log_entry("The orc slices you with a rusty sword (3 HP damage).");
                        player.hit_points -= 3;
                    } break;
                    case DRAGON : { 
                        log_entry("The dragon swallows you whole.");
                        player.hit_points -= 100;
                    } break;
                }
            } else {
                position_t dest = mob.second.pos;
                if (mob.second.pos.x < player.pos.x) ++dest.x;
                if (mob.second.pos.x > player.pos.x) --dest.x;
                if (mob.second.pos.y < player.pos.y) ++dest.y;
                if (mob.second.pos.y > player.pos.y) --dest.y;
                const int mobidx = mapidx(dest.x, dest.y);
                if (!tile_solid[mobidx]) mob.second.pos = dest;
            }
        }
    }

    // Check for pick-ups
    if (tile_render[mapidx(player.pos.x, player.pos.y)].glyph == 236) {
        log_entry("You pick up the iron key.");
        player.has_iron_key = true;
        set_floor(mapidx(player.pos.x, player.pos.y));
    }
    if (tile_render[mapidx(player.pos.x, player.pos.y)].glyph == 237) {
        log_entry("You pick up the gold key.");
        player.has_gold_key = true;
        set_floor(mapidx(player.pos.x, player.pos.y));
    }
    if (tile_render[mapidx(player.pos.x, player.pos.y)].glyph == 229) {
        log_entry("You drink a lovely healing potion. It tastes of flower petals.");
        player.hit_points += 10;
        set_floor(mapidx(player.pos.x, player.pos.y));
    }
    if (tile_render[mapidx(player.pos.x, player.pos.y)].glyph == 205) {
        log_entry("You read the map of the entire dungeon, and commit it to memory.");
        std::fill(tile_revealed.begin(), tile_revealed.end(), true);
        set_floor(mapidx(player.pos.x, player.pos.y));
    }
}

void play_mode_t::collide(const position_t &pos) {
    const int idx = mapidx(pos.x, pos.y);
    if (tile_render[idx].glyph == '+') {
        tile_render[idx].glyph = '.';
        tile_solid[idx] = false;
        log_entry("With a creak, the waterlogged door swings open. That's not closing again.");
    } else if (tile_render[idx].glyph == '!') {
        if (player.has_iron_key) {
            tile_render[idx].glyph = '.';
            tile_solid[idx] = false;
            player.has_iron_key = false;
            log_entry("You unlock the iron door with the iron key. It slides into the floor.");
        } else {
            log_entry("This door is solid iron, and requires the iron key to open.");
        }
    } else if (tile_render[idx].glyph == 240) {
        if (player.has_gold_key) {
            tile_render[idx].glyph = '.';
            tile_solid[idx] = false;
            player.has_gold_key = false;
            log_entry("You unlock the gold door with the gold key. It slides into the floor.");
        } else {
            log_entry("This door is solid gold, and requires the gold key to open.");
        }
    } else {
        log_entry("Ouch! That's a solid wall!");
    }
}

void play_mode_t::render_map(window_t * win) {
    // Map render
    win->cls();
    const int left_x = std::max(0, player.pos.x - (terminal_width/2)+15);
    const int top_y = std::max(0, player.pos.y - (terminal_height/2));
    int screen_y = 1;
    for (int y=top_y; y<top_y + terminal_height - 1; ++y) {
        int screen_x = 0;
        for (int x=left_x; x<left_x + terminal_width - 30; ++x) {
            if (x < level_width && y < level_height) {
                const int idx = mapidx(x, y);
                if (tile_revealed[idx]) {
                    uint8_t glyph = tile_render[idx].glyph;
                    uint8_t r = tile_render[idx].r;
                    uint8_t g = tile_render[idx].g;
                    uint8_t b = tile_render[idx].b;

                    if (!tile_visible[idx]) {
                        r /= 2;
                        g /= 2;
                        b /= 2;
                    }
                    win->set(screen_x, screen_y, glyph, r, g, b);

                    if (x == amulet.pos.x && y == amulet.pos.y) {
                        win->set(screen_x, screen_y, 157, 0, 255, 0);
                    }
                    auto trapfinder = traps.find(idx);
                    if (trapfinder != traps.end() && trapfinder->second.revealed) {
                        if (trapfinder->second.type == PIT) {
                            win->set(screen_x, screen_y, 127, 0, 255, 0);
                        } else if (trapfinder->second.type == BLADE) {
                            win->set(screen_x, screen_y, 241, 0, 255, 0);
                        }
                    }
                    
                    for (const auto &mob : monsters) {
                        if (mob.second.pos.x == x && mob.second.pos.y == y) {
                            win->set(screen_x, screen_y, mob.second.glyph, mob.second.r, mob.second.g, mob.second.b);
                        }
                    }
                    
                    if (x == player.pos.x && y == player.pos.y) {
                        win->set(screen_x, screen_y, '@', 255, 255, 0);
                    }
                }
            }
            ++screen_x;
        }
        ++screen_y;
    }

    const int term_left = terminal_width - 30;
    if (playtesting) {
        win->print(term_left, 0, "Prove dungeon is beatable!");
        win->print(term_left, 1, "ESCape back to the designer", 128, 128, 128);
    } else {
        win->print_center(0, levelName + std::string(" by ") + levelCreator, 255, 255, 0);
    }
    win->print(term_left, 3, std::string("Turn: ") + std::to_string(turn), 128, 128, 128);
    win->print(term_left, 4, std::string("HP: ") + std::to_string(player.hit_points), 128, 128, 128);
    win->set(term_left, 6, 157, 0, 255, 0);
    win->print(term_left+2, 6, "Find the Amulet of Winning", 128, 128, 128);
    win->print(term_left, 7, "Move with cursor keys,", 128, 128, 128);
    win->print(term_left, 8, "Numeric Keypad,", 128, 128, 128);
    win->print(term_left, 9, "or VI keys", 128, 128, 128);
    win->print(term_left, 10, ". Wait/Look for traps", 128, 128, 128);

    if (player.has_iron_key) {
        win->print(term_left, 12, "You have the iron key");
    }
    if (player.has_gold_key) {
        win->print(term_left, 13, "You have the gold key");
    }

    int y = terminal_height - 6;
    for (int i=0; i<5; ++i) {
        win->print(1, y + i, log[i], 128, 128, 128);
    }
}

void play_mode_t::log_entry(std::string msg) {
    log[4] = log[3];
    log[3] = log[2];
    log[2] = log[1];
    log[1] = log[0];
    log[0] = msg;
}
