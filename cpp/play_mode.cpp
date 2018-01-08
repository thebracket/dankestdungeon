#include "play_mode.hpp"
#include "rltk/visibility.hpp"
#include "components/position.hpp"
#include <algorithm>
#include <vector>

play_mode_t::play_mode_t(const level_t * level) {
    // Playtest constructor - copy the level to the playable format.

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
            set_floor(i);
            // TODO: Kobold
        } else if (tile_render[i].glyph == 'o') {
            set_floor(i);
            // TODO: Orc
        } else if (tile_render[i].glyph == 'D') {
            set_floor(i);
            // TODO: Dragon
        } else if (tile_render[i].glyph == 127) {
            set_floor(i);
            // TODO: Pit trap
        } else if (tile_render[i].glyph == 241) {
            set_floor(i);
            // TODO: Blade trap
        } else if (tile_render[i].glyph == 205) {
            set_floor(i);
            // TODO: Map scroll
        } else if (tile_render[i].glyph == 229) {
            set_floor(i);
            // TODO: Healing potion
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
        case '~': return true;
        default: return true;
    }
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
    if (win->is_key_down(SDLK_ESCAPE)) {
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

void play_mode_t::do_turn(input_type_t &input) {
    // Player
    bool player_moved = false;    
    if (input == LEFT && player.pos.x > 1) {
        if (!tile_solid[mapidx(player.pos.x - 1, player.pos.y)]) {
            --player.pos.x;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            --dest.x;
            collide(dest);
        }
    } else if (input == RIGHT && player.pos.x < level_width - 1) {
        if (!tile_solid[mapidx(player.pos.x + 1, player.pos.y)]) {
            ++player.pos.x;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            ++dest.x;
            collide(dest);
        }
    } else if (input == UP && player.pos.y > 1) {
        if (!tile_solid[mapidx(player.pos.x, player.pos.y-1)]) {
            --player.pos.y;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            --dest.y;
            collide(dest);
        }
    } else if (input == DOWN && player.pos.y < level_height - 1) {
        if (!tile_solid[mapidx(player.pos.x, player.pos.y+1)]) {
            ++player.pos.y;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            ++dest.y;
            collide(dest);
        }
    } else if (input == UPLEFT && player.pos.x > 1 && player.pos.y > 1) {
        if (!tile_solid[mapidx(player.pos.x-1, player.pos.y-1)]) {
            --player.pos.y;
            --player.pos.x;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            --dest.y;
            --dest.x;
            collide(dest);
        }
    } else if (input == UPRIGHT && player.pos.x < level_width-1 && player.pos.y > 1) {
        if (!tile_solid[mapidx(player.pos.x+1, player.pos.y-1)]) {
            --player.pos.y;
            ++player.pos.x;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            --dest.y;
            ++dest.x;
            collide(dest);
        }
    } else if (input == DOWNLEFT && player.pos.x > 1 && player.pos.y < level_height-1) {
        if (!tile_solid[mapidx(player.pos.x-1, player.pos.y+1)]) {
            ++player.pos.y;
            --player.pos.x;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            ++dest.y;
            --dest.x;
            collide(dest);
        }
    } else if (input == DOWNRIGHT && player.pos.x < level_width-1 && player.pos.y < level_height-1) {
        if (!tile_solid[mapidx(player.pos.x+1, player.pos.y+1)]) {
            ++player.pos.y;
            ++player.pos.x;
            player_moved = true;
        } else {
            position_t dest = player.pos;
            ++dest.y;
            ++dest.x;
            collide(dest);
        }
    }

    // Redo visibility after movement
    if (player_moved) cast_visibility();

    // Update counters
    ++turn;

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

    // Check for win/loss conditions
    if (player.pos.x == amulet.pos.x && player.pos.y == amulet.pos.y) {
        printf("Player was won the game in %d turns.\n", turn);
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

                    if (x == player.pos.x && y == player.pos.y) {
                        win->set(screen_x, screen_y, '@', 255, 255, 0);
                    }
                    if (x == amulet.pos.x && y == amulet.pos.y) {
                        win->set(screen_x, screen_y, 157, 0, 255, 0);
                    }
                }
            }
            ++screen_x;
        }
        ++screen_y;
    }

    const int term_left = terminal_width - 30;
    win->print(term_left, 0, "Dungeon Title");
    win->print(term_left, 2, std::string("Turn: ") + std::to_string(turn), 128, 128, 128);
    win->print(term_left, 3, std::string("HP: ") + std::to_string(player.hit_points), 128, 128, 128);
    win->set(term_left, 5, 157, 0, 255, 0);
    win->print(term_left+2, 5, "Find the Amulet of Winning", 128, 128, 128);
    win->print(term_left, 6, "Move with cursor keys,", 128, 128, 128);
    win->print(term_left, 7, "Numeric Keypad,", 128, 128, 128);
    win->print(term_left, 8, "or VI keys", 128, 128, 128);
    win->print(term_left, 9, ". Waits", 128, 128, 128);

    if (player.has_iron_key) {
        win->print(term_left, 11, "You have the iron key");
    }
    if (player.has_gold_key) {
        win->print(term_left, 12, "You have the gold key");
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
