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
    obj.id = *reinterpret_cast<uint32_t*>(data + pos); pos += sizeof(uint32_t);
    obj.pos.x = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    obj.pos.y = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    obj.scale.x = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    obj.scale.y = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    obj.color.r = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    obj.color.g = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    obj.color.b = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
    currentSnapshot.objects.insert(std::pair(obj.id, obj));
  }

  return EM_TRUE;
}

void sendServerEvent(server_event ev) {
  buffer[0] = static_cast<uint8_t>(opcodes::SERVER_EVENT);
  buffer[1] = ev;
  emscripten_websocket_send_binary(currentSocket, buffer, 2);
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
        // obj.pos.x = last.pos.x;
        // obj.pos.y = last.pos.y;
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

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(512, 512, 0, &window, &renderer);
  
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
  emscripten_set_main_loop(gameLoop, 0, 1);

  printf("end of main\n");

  return 0;
}