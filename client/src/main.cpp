#include <SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/websocket.h>

#include <stdlib.h>
#include <vector>
#include <mutex>
#include <cstddef>
#include <cstdlib>
#include <iostream>

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
  vec2 pos;
  vec2 scale;
  color color;
};

std::mutex stateMutex;
#define ACQUIRE_STATE_LOCK std::lock_guard<std::mutex> lock(stateMutex)

std::vector<object> objects;

SDL_Window *window;
SDL_Renderer *renderer;

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
  EMSCRIPTEN_RESULT result;
  result = emscripten_websocket_send_utf8_text(websocketEvent->socket, "join");
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
  {
    ACQUIRE_STATE_LOCK;

    if (websocketEvent->isText) {
      return EM_TRUE;
    }

    objects.clear();
    std::byte* data = reinterpret_cast<std::byte*>(websocketEvent->data);
    uint32_t numBytes = websocketEvent->numBytes;
    int numObjects = *reinterpret_cast<int*>(data);
    std::cout << numObjects << std::endl;
    size_t pos = sizeof(numObjects);
    for (int i = 0; i < numObjects; i++) {
      // deserialize an object
      object obj;
      obj.pos.x = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      obj.pos.y = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      obj.scale.x = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      obj.scale.y = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      obj.color.r = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      obj.color.g = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      obj.color.b = *reinterpret_cast<float*>(data + pos); pos += sizeof(float);
      std::cout << obj.pos.x << ", " << obj.pos.y << "  " << obj.scale.x << " x " << obj.scale.y << "  (" << obj.color.r << ", " << obj.color.g << ", " << obj.color.b << ")" << std::endl;
      objects.push_back(obj);
    }
  }

  return EM_TRUE;
}

void gameLoop() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  {
    ACQUIRE_STATE_LOCK;
    
    for (const auto& obj : objects) {
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