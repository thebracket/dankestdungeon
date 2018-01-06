#include <stdio.h>
#include "rltk/window.hpp"
#include <emscripten.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include "modestack.hpp"
#include "main_menu.hpp"

static std::unique_ptr<window_t> mainWindow;
static int counter = 0;

void loop_fn(void *arg) {
  if (!modestack.empty()) {
    mainWindow->poll();
    const bool should_pop = modestack.top()->tick(mainWindow.get());
    mainWindow->present();

    if (should_pop) {
      modestack.pop();
    }
  }
}

int main()
{
  mainWindow = std::make_unique<window_t>();
  modestack.emplace(std::make_unique<main_menu_t>());
  emscripten_set_main_loop_arg(loop_fn, nullptr, -1, 1);

  return 0;
}

