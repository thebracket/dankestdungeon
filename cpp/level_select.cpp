#include "level_select.hpp"
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <vector>
#include <string>
#include "modestack.hpp"
#include "play_mode.hpp"

bool got_levels = false;

struct available_level_t {
    int id;
    std::string name;
    std::string creator;
};

std::vector<available_level_t> available_levels;

level_t level;

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    using namespace std;
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

bool got_level = false;

void gotLevel(emscripten_fetch_t *fetch) {
    if (got_level) return;
    std::fill(level.tiles.begin(), level.tiles.end(), tile_t{'.', 128, 128, 128});

    std::string tmp = "";
    for (int i=1; i<fetch->numBytes-1; ++i) {
        tmp += fetch->data[i];
    }

    int place = 0;
    int tilecount = 0;
    auto splits = split(tmp, "X");
    for (const auto &tile : splits) {
        switch (place) {
        case 0 : level.tiles[tilecount].glyph = std::stoi(tile); break;
        case 1 : level.tiles[tilecount].r = std::stoi(tile); break;
        case 2 : level.tiles[tilecount].g = std::stoi(tile); break;
        case 3 : level.tiles[tilecount].b = std::stoi(tile); break;
        }
        ++place;
        if (place > 3) {
            place = 0;
            ++tilecount;
        }
    }

    got_level = true;
    modestack.emplace(std::make_unique<play_mode_t>(&level, false));
}

bool level_select_t::tick(window_t * win) {
    if (got_level) {
        // If we've already loaded a level, skip us on the stack
        got_level = false;
        return true;
    }

    if (first_run) {
        first_run = false;
        get_levels();
    }

    win->cls();
    win->print_center(1, "Select Your Dungeon", 255, 255, 0);
    win->print_center(2, "ESC to return to main menu", 128, 128, 128);

    int start = 0;

    if (got_levels) {
        int count = 0;
        int y = 4;
        for (const auto &lvl : available_levels) {
            if (count >= start && y < terminal_height-6) {
                std::string display = lvl.name + ", " + lvl.creator;
                if (win->mouse_y == y) {
                    win->print_center(y, display, 255, 255, 0);
                } else {
                    win->print_center(y, display);
                }

                if (win->clicked && win->mouse_y == y) {
                    level.id = lvl.id;
                    level.name = lvl.name;
                    level.creator = lvl.creator;

                    // Load the level via AJAX call
                    std::string api_name = "/api/Dungeon/" + std::to_string(lvl.id);
                    emscripten_fetch_attr_t attr;
                    emscripten_fetch_attr_init(&attr);
                    strcpy(attr.requestMethod, "GET");
                    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                    attr.onsuccess = gotLevel;
                    //attr.onerror = downloadFailed;
                    emscripten_fetch(&attr, api_name.c_str()); 
                    // This will call into gotLevel when complete
                }
            }
            ++y;
            ++count;
        }
    } else {
        win->print_center(4, "Please wait - downloading levels");
    }

    if (win->is_key_down(SDLK_ESCAPE)) {
        return true;
    }
    if (win->is_key_down(SDLK_PAGEDOWN)) {
        start += terminal_height-6;
    }
    if (win->is_key_down(SDLK_PAGEUP)) {
        start -= terminal_height-6;
    }
    return false;
}

void gotLevels(emscripten_fetch_t *fetch) {
    got_levels = true;

    std::string tmp = "";
    for (int i=1; i<fetch->numBytes-1; ++i) {
        tmp += fetch->data[i];
    }

    int place = 0;
    auto splits = split(tmp, "\\n");
    available_level_t lvl;
    for (const auto &line : splits) {
        if (!line.empty()) {
            if (place == 0) {
                lvl.id = std::stoi(line);
            } else if (place == 1) {
                lvl.name = line;
            } else {
                lvl.creator = line;
                available_levels.push_back(lvl);
            }
        }
        ++place;
        if (place > 2) place = 0;
    }
}

void level_select_t::get_levels() {
    available_levels.clear();

    std::string api_name = "/api/DungeonList";
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = gotLevels;
    //attr.onerror = downloadFailed;
    emscripten_fetch(&attr, api_name.c_str());  
}
