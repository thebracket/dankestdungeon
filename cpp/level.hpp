#pragma once

#include <array>

constexpr int level_width = 100;
constexpr int level_height = 100;
constexpr int level_tiles = level_width * level_height;

struct tile_t {
    uint8_t glyph = 176;
    uint8_t r = 128;
    uint8_t g = 128;
    uint8_t b = 128;
};

struct level_t {
    std::array<tile_t, level_tiles> tiles;
    std::string name;
    int id;

    inline int idx(const int &x, const int &y) {
        return (y * level_width) + x;
    }
};
