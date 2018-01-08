#pragma once

struct position_t {
    uint8_t x;
    uint8_t y;
};

struct navigator_t {
    static inline int get_x(position_t pos) { return pos.x; }
    static inline int get_y(position_t pos) { return pos.y; }
    static inline position_t get_xy(int x, int y) { return position_t{static_cast<uint8_t>(x), static_cast<uint8_t>(y)}; }
};
