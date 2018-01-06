#include "window.hpp"
#include <algorithm>

window_t::window_t() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(terminal_width * font_size, terminal_height * font_size, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    font = std::make_unique<sprite_t>(renderer, "assets/terminal16x16.png");

    cls();
}

window_t::~window_t() {

}

void window_t::present() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    int x = 0;
    int y = 0;
    for (int i=0; i<terminal_width * terminal_height; ++i) {
        uint8_t glyph = console[i];
        const int src_x = (glyph % 16) * font_size;
        const int src_y = (glyph / 16) * font_size;
        const uint8_t red = colors[i * 3];
        const uint8_t green = colors[(i * 3)+1];
        const uint8_t blue = colors[(i * 3)+2];
        font->render_subsprite(renderer, font_size, font_size, x*font_size, y*font_size, src_x, src_y, red, green, blue );

        ++x;
        if (x >= terminal_width) {
            x = 0;
            ++y;
        }
    }

    SDL_RenderPresent(renderer);
}

void window_t::poll() {
    keypress.clear();

    SDL_Event event;
    while( SDL_PollEvent( &event ) ){
        switch( event.type ){
            case SDL_KEYDOWN:
            keypress.insert(event.key.keysym.sym);
                break;

            case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)  {
                clicked = true;
            } break;

            case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)  {
                clicked = false;
            } break;

            case SDL_MOUSEMOTION: {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

                mouse_x = mouseX / font_size;
                mouse_y = mouseY / font_size;
            } break;

            default:
                break;
        }
    }
}

bool window_t::is_key_down(const int &symbol) {
    auto finder = keypress.find(symbol);
    if (finder == keypress.end()) return false;
    return true;
}

void window_t::cls() {
    std::fill(console.begin(), console.end(), ' ');
    std::fill(colors.begin(), colors.end(), 255);
}

int window_t::loc(const int &x, const int &y) {
    return (y * terminal_width) + x;
}

void window_t::set(const int &x, const int &y, const uint8_t &glyph, int r, int g, int b) {
    const int idx = loc(x,y);
    const int idx3 = idx*3;
    console[idx] = glyph;
    colors[idx3] = r;
    colors[idx3+1] = g;
    colors[idx3+2] = b;
}

void window_t::print(const int &x, const int &y, const std::string &msg, int r, int g, int b) {
    int idx = loc(x,y);
    for (int i=0; i<msg.size(); ++i) {
        const int IDX = idx+i;
        const int IDX3 = IDX * 3;
        console[IDX] = msg[i];
        colors[IDX3] = r;
        colors[IDX3 + 1] = g;
        colors[IDX3 + 2] = b;
    }
}

void window_t::print_center(const int &y, const std::string &msg, int r, int g, int b) {
    const int x = (terminal_width/2) - (msg.size() / 2);
    print(x, y, msg, r, g, b);
}

void window_t::box(const int x, const int y, const int w, const int h, int r, int g, int b, bool double_lines) {
    for (int i=1; i<w; ++i) {
		if (!double_lines) {
			set(x+i, y, 196, r, g, b);
			set(x+i, y+h, 196, r, g, b);
		} else {
			set(x+i, y, 205, r, g, b);
			set(x+i, y+h, 205, r, g, b);
		}
	}
	
	for (int i=1; i<h; ++i) {
		if (!double_lines) {
			set(x, y+i, 179, r, g, b);
			set(x+w, y+i, 179, r, g, b);
		} else {
			set(x, y+i, 186, r, g, b);
			set(x+w, y+i, 186, r, g, b);
		}
	}
	
	if (!double_lines) {
		set(x,y, 218, r, g, b);
		set(x+w, y, 191, r, g, b);
		set(x, y+h, 192, r, g, b);
		set(x+w, y+h, 217, r, g, b);
	} else {
		set(x,y, 201, r, g, b);
		set(x+w, y, 187, r, g, b);
		set(x, y+h, 200, r, g, b);
		set(x+w, y+h, 188, r, g, b);
    }
}

