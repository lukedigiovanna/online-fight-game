#include <SDL_ttf.h>
#include <SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/websocket.h>

#include <stdlib.h>
#include <vector>
#include <map>
#include <mutex>
#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "../../common/opcodes.h"
#include "../../common/serialization.h"

#include "renderutils.h"
#include "font.h"
#include "ui.h"
#include "utils.h"
#include "screen.h"
#include "models.h"
#include "client.h"
#include "runtimeinfo.h"

SDL_Window *window;
SDL_Renderer *renderer;

void resizeCanvas() {
    double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    emscripten_set_canvas_element_size("#canvas", (int)width, (int)height);
  
    SDL_SetWindowSize(window, (int)width, (int)height);
    SDL_RenderSetLogicalSize(renderer, (int)width, (int)height);
    RuntimeInfo::updateDisplayDimensions((int)width, (int)height);
}

EM_BOOL resizeCanvas_callback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    resizeCanvas();
  
    return EM_TRUE;
}

vec2 mousePos = {};

MainScreen mainScreen;
GameScreen gameScreen;

void mainLoop() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  ScreenManager::render(renderer);

  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    if (ev.type == SDL_MOUSEMOTION) {
      mousePos.x = ev.motion.x;
      mousePos.y = ev.motion.y;
    }
    ScreenManager::processEvent(ev);
  }

  SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(512, 512, 0, &window, &renderer);
    
    if (TTF_Init()) {
        std::cerr << "ERROR: main: " << TTF_GetError() << std::endl;
        return 1;
    }

    ScreenManager::registerScreen("main", &mainScreen);
    ScreenManager::registerScreen("game", &gameScreen);
  
    Fonts::registerFont("Roboto", "assets/Roboto-Regular.ttf");
    Fonts::registerFont("Roboto-Bold", "assets/Roboto-Bold.ttf");
  
    ScreenManager::loadScreen("main");
  
    resizeCanvas(); 
  
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_FALSE, resizeCanvas_callback);
    
    printf("launching game loop\n");
    emscripten_set_main_loop(mainLoop, 0, 1);

    return 0;
}
