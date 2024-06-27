#pragma once

#include <stdlib.h>
#include <string>
#include <mutex>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/websocket.h>

#include <SDL.h>

#include "../../common/opcodes.h"

#include "models.h"

#define BUFFER_SIZE 256

// static class to handle the client

class Client {
private:
    static uint8_t buffer[BUFFER_SIZE];

    static EMSCRIPTEN_WEBSOCKET_T ws;
    static int currentSocket;

    static snapshot currentSnapshot;
    static snapshot lastSnapshot;

    static std::mutex stateMutex;
public:
    static void connect();

    static void joinGame(const std::string& roomCode);

    static void gameLoop();
    
    static void processSDLEvent(const SDL_Event& ev);

    static void render(SDL_Renderer* renderer);

    inline static void sendServerEvent(server_event ev) {
        buffer[0] = static_cast<uint8_t>(opcodes::SERVER_EVENT);
        buffer[1] = ev;
        emscripten_websocket_send_binary(currentSocket, buffer, 2);
    }
private:
    static EM_BOOL onopen(
        int eventType, 
        const EmscriptenWebSocketOpenEvent *websocketEvent, 
        void *userData);
    static EM_BOOL onmessage(
        int eventType, 
        const EmscriptenWebSocketMessageEvent *websocketEvent, 
        void *userData);
};