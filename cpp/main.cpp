#include <stdio.h>
#include "rltk/window.hpp"
#include <emscripten.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include "modestack.hpp"
#include "main_menu.hpp"
#include <emscripten/fetch.h>

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

void downloadSucceeded(emscripten_fetch_t *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.

  username = "";
  for (int i=1; i<fetch->numBytes-1; ++i) {
    username += fetch->data[i];
  }

  modestack.emplace(std::make_unique<main_menu_t>());
  emscripten_set_main_loop_arg(loop_fn, nullptr, -1, 1);
}

void downloadFailed(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

int main()
{
  mainWindow = std::make_unique<window_t>();

  // Get the token ID# from local storage
  token = EM_ASM_INT(
    return localStorage.getItem("dankest_token");
  );

  std::string api_name = "/api/StartGame/" + std::to_string(token);
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = downloadSucceeded;
  attr.onerror = downloadFailed;
  emscripten_fetch(&attr, api_name.c_str());  

  return 0;
}

