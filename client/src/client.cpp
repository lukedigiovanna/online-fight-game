#include "client.h"

#include <functional>

#include "utils.h"
#include "renderutils.h"

#include "../../common/serialization.h"

#define ACQUIRE_STATE_LOCK std::lock_guard<std::mutex> lock(stateMutex)

uint8_t Client::buffer[BUFFER_SIZE];

EMSCRIPTEN_WEBSOCKET_T Client::ws;

int Client::currentSocket;

snapshot Client::currentSnapshot;
snapshot Client::lastSnapshot;

std::mutex Client::stateMutex;

void Client::connect() {
    if (!emscripten_websocket_is_supported()) {
        throw std::runtime_error("Websocket is not supported");
    }

    EmscriptenWebSocketCreateAttributes ws_attrs = {
        "ws://localhost:9002",
        NULL,
        EM_TRUE
    };

    ws = emscripten_websocket_new(&ws_attrs);

    emscripten_websocket_set_onopen_callback(ws, NULL, Client::onopen);
    emscripten_websocket_set_onmessage_callback(ws, NULL, Client::onmessage);
}

void Client::gameLoop() {
    ACQUIRE_STATE_LOCK;

    float now = utils::getTime();
    float diff = currentSnapshot.timestamp - lastSnapshot.timestamp;
    float p = 0;
    if (diff > 0) {
      p = (now - currentSnapshot.timestamp) / diff;
    }

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
    }
}

void Client::processSDLEvent(const SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
            case SDLK_w:
            sendServerEvent(server_event::JUMP); break;
            case SDLK_a:
            sendServerEvent(server_event::START_MOVE_LEFT); break;
            case SDLK_d:
            sendServerEvent(server_event::START_MOVE_RIGHT); break;
        }
    }
    else if (ev.type == SDL_KEYUP) {
        switch (ev.key.keysym.sym) {
            case SDLK_a:
            sendServerEvent(server_event::END_MOVE_LEFT); break;
            case SDLK_d:
            sendServerEvent(server_event::END_MOVE_RIGHT); break;
        }
    }
}

void Client::render(SDL_Renderer* renderer) {
    ACQUIRE_STATE_LOCK;

    for (const auto [id, obj] : currentSnapshot.objects) {
        SDL_FRect rect = { obj.pos.x, obj.pos.y, obj.scale.x, obj.scale.y };
        renderutils::setRenderColor(renderer, colors::BLUE);
        SDL_RenderDrawRectF(renderer, &rect);
    }
}

EM_BOOL Client::onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
    // when we open the socket, attempt to join the game
    currentSocket = websocketEvent->socket;
    *(reinterpret_cast<uint8_t*>(buffer)) = opcodes::JOIN;
    EMSCRIPTEN_RESULT result = emscripten_websocket_send_binary(currentSocket, buffer, 1);
    if (result) {
        printf("Failed to emscripten_websocket_send_binary(): %d\n", result);
    }
    return EM_TRUE;
}

EM_BOOL Client::onmessage(
    int eventType, 
    const EmscriptenWebSocketMessageEvent *websocketEvent, 
    void *userData) {

    if (websocketEvent->isText) {
        return EM_TRUE;
    }
    
    // ACQUIRE_STATE_LOCK;

    // make the last the current
    lastSnapshot.timestamp = currentSnapshot.timestamp;
    lastSnapshot.objects = currentSnapshot.objects;

    float now = utils::getTime();

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