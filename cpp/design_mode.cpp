#include "design_mode.hpp"
#include "play_mode.hpp"
#include "modestack.hpp"
#include <algorithm>
#include <emscripten.h>

bool design_mode_t::tick(window_t * win) {
    win->cls();
    win->print_center(0, "Build a dungeon for other players", 255, 255, 0);

    if (first_run) {
        initialize_dungeon();
        first_run = false;
    }

    render_map(win);
    render_ui(win);
    handle_inputs(win);

    if (win->is_key_down(SDLK_ESCAPE) && game_exit_state == WON) {
        // Build it into a persistent format
        std::string level_data = "";
        for (int i=0; i<level_tiles; ++i) {
            level_data += level.tiles[i].glyph;
            level_data += level.tiles[i].r;
            level_data += level.tiles[i].g;
            level_data += level.tiles[i].b;
        }

        // We need to title the dungeon
        EM_ASM(
            var level_data = Pointer_stringify($0);
            var dungeonName = "";
            while (dungeonName == "") {
                dungeonName = prompt("Please name your dungeon", localStorage.getItem("dankest_user") + "'s Dungeon");
            }
            saveLevel(dungeonName, level_data);
        , level_data.c_str());

        // Post it to the server
        // Break out!
        return true;
    }
    return false;
}

void design_mode_t::initialize_dungeon() {
    std::fill(level.tiles.begin(), level.tiles.end(), tile_t{ 176, 128, 128, 128 });

    for (int y=(level_height/2)-5; y < (level_height / 2)+5; ++y) {
        for (int x=(level_width/2)-5; x < (level_width/2)+5; ++x) {
            level.tiles[level.idx(x,y)] = tile_t{46, 128, 128, 128};
        }
    }
    level.tiles[level.idx(level_width / 2, level_height / 2)] = tile_t{64, 255, 255, 0 };
    level.tiles[level.idx((level_width / 2) - 4, level_height / 2)] = tile_t{157, 0, 255, 0 };
};

void design_mode_t::render_map(window_t * win) {
    int found_player = 0;
    int found_amulet = 0;

    const int left_x = std::max(0, cx - (terminal_width/2));
    const int top_y = std::max(0, cy - (terminal_height/2));

    int screen_y = 1;
    for (int y=top_y; y<top_y + terminal_height - 1; ++y) {
        int screen_x = 0;
        for (int x=left_x; x<left_x + terminal_width - 20; ++x) {
            if (x < level_width && y < level_height) {
                const int idx = level.idx(x, y);
                uint8_t glyph = level.tiles[idx].glyph;
                win->set(screen_x, screen_y, glyph, level.tiles[idx].r, level.tiles[idx].g, level.tiles[idx].b);
                if (glyph == '@') ++found_player;
                if (glyph == 157) ++found_amulet;
            }
            ++screen_x;
        }
        ++screen_y;
    }

    if (found_player == 0) win->print_center(terminal_height - 2, "Map must include a player!", 255, 0, 0);
    if (found_player > 1) win->print_center(terminal_height - 2, "Map must have only one player", 255, 0, 0);
    if (found_amulet == 0) win->print_center(terminal_height - 1, "Map must include the amulet of winning!", 255, 0, 0);
    if (found_amulet > 1) win->print_center(terminal_height - 1, "Map must have only one amulet of winning!", 255, 0, 0);

    if (game_exit_state == DEAD) {
        win->print_center(0, "Last attempt at the dungeon resulted in painful death! Tweak and try again.", 255, 0, 0);
    } else if (game_exit_state == WON) {
        win->print_center(0, "You have beaten the dungeon - press ESC to publish.", 0, 255, 0);        
    }
}

void design_mode_t::render_ui(window_t * win) {
    const int panel_left = terminal_width - 19;

    win->print(panel_left, 1, "Arrow keys scroll", 128, 128, 128);

    if (mode == TERRAIN) {
        win->print(panel_left, 3, "Terrain", 255, 0, 0);
    } else {
        win->print(panel_left, 3, "(T)errain", 128, 0, 0);        
    }
    if (mode == SPECIAL) {
        win->print(panel_left, 4, "Required", 255, 0, 0);
    } else {
        win->print(panel_left, 4, "(R)equired", 128, 0, 0);        
    } 
    if (mode == PUZZLE) {
        win->print(panel_left, 5, "Puzzle", 255, 0, 0);
    } else {
        win->print(panel_left, 5, "(P)uzzle", 128, 0, 0);        
    } 
    if (mode == MOB) {
        win->print(panel_left, 6, "Monster", 255, 0, 0);
    } else {
        win->print(panel_left, 6, "(M)onster", 128, 0, 0);        
    } 
    if (mode == TRAP) {
        win->print(panel_left, 7, "Trap", 255, 0, 0);
    } else {
        win->print(panel_left, 7, "(1)Trap", 128, 0, 0);        
    }
    if (mode == PERKS) {
        win->print(panel_left, 8, "Perks", 255, 0, 0);
    } else {
        win->print(panel_left, 8, "(3)Perks", 128, 0, 0);        
    }

    if (mode == TERRAIN) {
        render_submenu(win, &terrain_elements);
    } else if (mode == SPECIAL) {
        render_submenu(win, &special_elements);
    } else if (mode == PUZZLE) {
        render_submenu(win, &puzzle_elements);
    } else if (mode == MOB) {
        render_submenu(win, &mob_elements);
    } else if (mode == TRAP) {
        render_submenu(win, &trap_elements);
    } else if (mode == PERKS) {
        render_submenu(win, &perk_elements);
    }

    if (win->is_key_down(SDLK_t)) mode = TERRAIN;
    if (win->is_key_down(SDLK_r)) mode = SPECIAL;
    if (win->is_key_down(SDLK_p)) mode = PUZZLE;
    if (win->is_key_down(SDLK_m)) mode = MOB;
    if (win->is_key_down(SDLK_1)) mode = TRAP;
    if (win->is_key_down(SDLK_3)) mode = PERKS;
    if (win->clicked) {
            if (win->mouse_x > terminal_width - 20) {
                if (win->mouse_y == 3) mode = TERRAIN;
                if (win->mouse_y == 4) mode = SPECIAL;
                if (win->mouse_y == 5) mode = PUZZLE;
                if (win->mouse_y == 6) mode = MOB;
                if (win->mouse_y == 7) mode = TRAP;
                if (win->mouse_y == 8) mode = PERKS;
            }
    }

    win->print(panel_left, terminal_height-1, "Playtest >>>", 0, 255, 0);
    if (win->clicked) {
            if (win->mouse_x > terminal_width - 20) {
                if (win->mouse_y == terminal_height - 1) {
                    modestack.emplace(std::make_unique<play_mode_t>(&level));
                }
            }
    }
    if (win->is_key_down(SDLK_PERIOD)) {
        modestack.emplace(std::make_unique<play_mode_t>(&level));
    }
    
}

void design_mode_t::render_submenu(window_t * win, std::vector<design_element_t> * elements) {
    const int panel_left = terminal_width - 19;
    int y = 10;

    for (std::size_t i=0; i<elements->size(); ++i) {
        if (paint_glyph == elements->at(i).glyph) {
            win->print(panel_left, y, elements->at(i).title, 255, 255, 255);
        } else {
            win->print(panel_left, y, elements->at(i).title, 128, 128, 128);
        }

        if (win->is_key_down(elements->at(i).hotkey)) {
            paint_glyph = elements->at(i).glyph;
            paint_r = elements->at(i).r;
            paint_g = elements->at(i).g;
            paint_b = elements->at(i).b;
        }

        if (win->clicked) {
            if (win->mouse_x > terminal_width - 20) {
                if (win->mouse_y == y) {
                    paint_glyph = elements->at(i).glyph;
                    paint_r = elements->at(i).r;
                    paint_g = elements->at(i).g;
                    paint_b = elements->at(i).b;
                }
            }
        }

        ++y;
    }
}

void design_mode_t::handle_inputs(window_t * win) {
    // Process keyboard movement
    if (win->is_key_down(SDLK_LEFT)) {
        --cx;
        if (cx < 0) cx = 0;
    }
    if (win->is_key_down(SDLK_RIGHT)) {
        ++cx;
        if (cx > level_width - 1) cx = level_width - 1;
    }
    if (win->is_key_down(SDLK_UP)) {
        --cy;
        if (cy < 0) cy = 0;
    }
    if (win->is_key_down(SDLK_DOWN)) {
        ++cy;
        if (cy > level_height - 1) cy = level_height - 1;
    }

    if (win->clicked) {
        if (win->mouse_x < terminal_width - 20) {
            // We are in the map
            const int left_x = std::max(0, cx - (terminal_width/2));
            const int top_y = std::max(0, cy - (terminal_height/2) - 1);
            const int click_x = left_x + win->mouse_x;
            const int click_y = top_y + win->mouse_y;

            if (click_x > 0 && click_x < level_width && click_y > 0 && click_y < level_height) {
                const int idx = level.idx(click_x, click_y);
                level.tiles[idx].glyph = paint_glyph;
                level.tiles[idx].r = paint_r;
                level.tiles[idx].g = paint_g;
                level.tiles[idx].b = paint_b;
                game_exit_state = IN_PROGRESS;
            }
        }
    }
}