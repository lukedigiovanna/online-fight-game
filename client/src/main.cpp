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

struct vec2 {
  float x;
  float y;
};

struct color {
  float r;
  float g;
  float b;
};

struct object {
  uint32_t id;
  vec2 pos;
  vec2 scale;
  color color;
};

std::mutex stateMutex;
#define ACQUIRE_STATE_LOCK std::lock_guard<std::mutex> lock(stateMutex)

struct snapshot {
  float timestamp;
  std::map<uint32_t, object> objects;
};

#define BUFFER_SIZE 1024

uint8_t buffer[BUFFER_SIZE];

// interpolate between these two. (linear i suppose)
snapshot lastSnapshot;
snapshot currentSnapshot;

SDL_Window *window;
SDL_Renderer *renderer;

int currentSocket;

void resizeCanvas() {
  double width, height;
  emscripten_get_element_css_size("#canvas", &width, &height);
  emscripten_set_canvas_element_size("#canvas", (int)width, (int)height);

  SDL_SetWindowSize(window, (int)width, (int)height);
  SDL_RenderSetLogicalSize(renderer, (int)width, (int)height);
}

EM_BOOL resizeCanvas_callback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
  resizeCanvas();

  return EM_TRUE;
}

EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
  // when we open the socket, attempt to join the game
  currentSocket = websocketEvent->socket;
  *(reinterpret_cast<uint8_t*>(buffer)) = opcodes::JOIN;
  EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(currentSocket, buffer, 1);
  if (result) {
      printf("Failed to emscripten_websocket_send_utf8_text(): %d\n", result);
  }
  return EM_TRUE;
}
EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData) {
  // do nothing
  return EM_TRUE;
}
EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData) {
  // do nothing
  return EM_TRUE;
}
EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
  if (websocketEvent->isText) {
    return EM_TRUE;
  }
  
  ACQUIRE_STATE_LOCK;

  // make the last the current
  lastSnapshot.timestamp = currentSnapshot.timestamp;
  lastSnapshot.objects = currentSnapshot.objects;

  float now = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000.0f;

  currentSnapshot.timestamp = now;
  currentSnapshot.objects.clear();

  std::byte* data = reinterpret_cast<std::byte*>(websocketEvent->data);
  uint32_t numBytes = websocketEvent->numBytes;
  int numObjects = *reinterpret_cast<int*>(data);
  size_t pos = sizeof(numObjects);
  for (int i = 0; i < numObjects; i++) {
    // deserialize an object
    object obj;
    obj.id = serializer::read<size_t>(data, pos);
    obj.pos.x = serializer::read<float>(data, pos);
    obj.pos.y = serializer::read<float>(data, pos);
    obj.scale.x = serializer::read<float>(data, pos);
    obj.scale.y = serializer::read<float>(data, pos);
    obj.color.r = serializer::read<float>(data, pos);
    obj.color.g = serializer::read<float>(data, pos);
    obj.color.b = serializer::read<float>(data, pos);
    currentSnapshot.objects.insert(std::pair(obj.id, obj));
  }

  return EM_TRUE;
}

void sendServerEvent(server_event ev) {
  buffer[0] = static_cast<uint8_t>(opcodes::SERVER_EVENT);
  buffer[1] = ev;
  emscripten_websocket_send_binary(currentSocket, buffer, 2);
}

enum Screen {
  MAIN,
  GAME
};

Screen screen = MAIN;

vec2 mousePos = {};

ui::Button* increaseFontButton;
ui::Button* decreaseFontButton;

int fontSize = 50;

void increaseFontSize() {
  fontSize++;
}

void decreaseFontSize() {
  fontSize--;
  if (fontSize < 1) fontSize = 1;
}

void mainScreenLoop() {
  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
  SDL_RenderClear(renderer);

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (increaseFontButton->processSDLEvent(e)) {
      continue;
    }
    if (decreaseFontButton->processSDLEvent(e)) {
      continue;
    }
    if (e.type == SDL_MOUSEMOTION) {
      mousePos.x = e.motion.x;
      mousePos.y = e.motion.y;
    }
    else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          fontSize++; break;
        case SDLK_DOWN:
          fontSize--;
          if (fontSize < 1) fontSize = 1;
          break;
      }
    }
  }

  // draw a button
  SDL_FRect rect = { mousePos.x - 10, mousePos.y - 10, 20, 20 };
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRectF(renderer, &rect);

  TTF_Font* font = Fonts::getFont("Roboto", fontSize);

  renderutils::drawText(renderer, font, "abcdefghijklmnopqsrtuvwxyz", 50, 50, fontSize, colors::MAGENTA);
  renderutils::drawText(renderer, font, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 50, 50 + (fontSize + 5) * 1, fontSize, colors::GREEN);
  renderutils::drawText(renderer, font, "0123456789", 50, 50 + (fontSize + 5) * 2, fontSize, colors::BLUE);
  renderutils::drawText(renderer, font, "!@#$%^&*()", 50, 50 + (fontSize + 5) * 3, fontSize, colors::LIGHT_GRAY);
  renderutils::drawText(renderer, font, std::to_string(fontSize), 50, 50 + (fontSize + 5) * 4, fontSize, colors::WHITE);

  increaseFontButton->render(renderer);
  decreaseFontButton->render(renderer);

  SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
  renderutils::fillCircle(renderer, 250, 250, 50);
  renderutils::fillRoundedRect(renderer, 250, 450, 200, 50, 10);

  SDL_RenderPresent(renderer);
}

void gameLoop() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_w:
          sendServerEvent(server_event::JUMP); break;
        case SDLK_a:
          sendServerEvent(server_event::START_MOVE_LEFT); break;
        case SDLK_d:
          sendServerEvent(server_event::START_MOVE_RIGHT); break;
      }
    }
    else if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.sym) {
        case SDLK_a:
          sendServerEvent(server_event::END_MOVE_LEFT); break;
        case SDLK_d:
          sendServerEvent(server_event::END_MOVE_RIGHT); break;
      }
    }
  }

  {
    ACQUIRE_STATE_LOCK;
    
    float now = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000.0f;
    float diff = currentSnapshot.timestamp - lastSnapshot.timestamp;
    float p = 0;
    if (diff > 0) {
      p = (now - currentSnapshot.timestamp) / diff;
    }

    // std::cout << lastSnapshot.timestamp << " to " << currentSnapshot.timestamp << "(" << p * 100 << "%)" << std::endl;

    for (auto& [id, curr] : currentSnapshot.objects) {
      // interpolate from their last frame
      object obj = curr;

      auto it = lastSnapshot.objects.find(id);
      if (it != lastSnapshot.objects.end()) {
        object& last = it->second;
        // interpolate appropriately
        obj = {};
        obj.pos.x = last.pos.x + (curr.pos.x - last.pos.x) * p;
        obj.pos.y = last.pos.y + (curr.pos.y - last.pos.y) * p;
        // obj.scale.x = last.scale.x + (curr.scale.x - last.scale.x) * p;
        // obj.scale.y = last.scale.y + (curr.scale.y - last.scale.y) * p;
        obj.scale.x = curr.scale.x;
        obj.scale.y = curr.scale.y;
        obj.color.r = curr.color.r;
        obj.color.g = curr.color.g;
        obj.color.b = curr.color.b;
      }

      // std::cout << obj.pos.x << ", " << obj.pos.y << "    " << obj.scale.x << ", " << obj.scale.y << std::endl;

      SDL_FRect rect = { obj.pos.x, obj.pos.y, obj.scale.x, obj.scale.y };
      SDL_SetRenderDrawColor(renderer, obj.color.r, obj.color.g, obj.color.b, 255);
      SDL_RenderFillRectF(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
}

void mainLoop() {
  switch (screen) {
    case MAIN:
      mainScreenLoop();
      break;
    case GAME:
      gameLoop();
      break;
  }
}

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(512, 512, 0, &window, &renderer);
  
  if (TTF_Init()) {
    std::cerr << "ERROR: main: " << TTF_GetError() << std::endl;
    return 1;
  }

  Fonts::registerFont("Roboto", "assets/Roboto-Regular.ttf");

  increaseFontButton = new ui::Button("Roboto", "Increase", 250, 100, 20);
  increaseFontButton->setOnClick(increaseFontSize);
  decreaseFontButton = new ui::Button("Roboto", "Decrease", 366, 320, 30);
  decreaseFontButton->setOnClick(decreaseFontSize);

  resizeCanvas();

  printf("setting up websocket\n");

  if (!emscripten_websocket_is_supported()) {
        return 0;
  }
  EmscriptenWebSocketCreateAttributes ws_attrs = {
      "ws://localhost:9002",
      NULL,
      EM_TRUE
  };

  EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new(&ws_attrs);
  emscripten_websocket_set_onopen_callback(ws, NULL, onopen);
  emscripten_websocket_set_onerror_callback(ws, NULL, onerror);
  emscripten_websocket_set_onclose_callback(ws, NULL, onclose);
  emscripten_websocket_set_onmessage_callback(ws, NULL, onmessage);

  printf("launching game loop\n");
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_FALSE, resizeCanvas_callback);
  emscripten_set_main_loop(mainLoop, 0, 1);

  printf("end of main\n");

  return 0;
}