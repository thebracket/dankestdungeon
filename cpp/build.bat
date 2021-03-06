@echo Setting Environment
@call c:\users\herbert\Development\emscripten\emsdk_env.bat
@echo Building
@call emcc -std=c++1y -o3 -O3 --use-preload-plugins --preload-file assets -s FETCH=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -o bin/dankest.js main.cpp rltk/window.cpp rltk/sprite.cpp rltk/base_mode.cpp modestack.cpp main_menu.cpp leaderboards.cpp design_mode.cpp play_mode.cpp rltk/geometry.cpp level_select.cpp
@call copy bin\* ..\node\dankest-dungeon\public

