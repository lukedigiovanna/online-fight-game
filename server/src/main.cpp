#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_THREAD_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstddef>
#include <cstdlib>

#include "data.h"
#include "mathutils.h"

#include "../../common/opcodes.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using connection = std::shared_ptr<websocketpp::connection<websocketpp::config::asio>>;

std::atomic<bool> terminate = false;

std::mutex stateMutex;
std::map<connection, std::shared_ptr<object>> connections;

uint32_t lastObjectID = 0;
std::vector<std::shared_ptr<object>> objects;

server ws_server;

#define ACQUIRE_STATE_LOCK std::lock_guard<std::mutex> lock(stateMutex)

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    std::string payload = msg->get_payload();

    const char* data = payload.data();

    uint8_t opcode = static_cast<uint8_t>(*data);

    std::cout << "got opcode: " << opcode << std::endl;

    std::shared_ptr conn = s->get_con_from_hdl(hdl);
    if (opcode == opcodes::JOIN) {
        std::cout << "got JOIN opcode" << std::endl;
        {
            ACQUIRE_STATE_LOCK;
            std::shared_ptr<object> o = std::make_shared<object>(object{
                ++lastObjectID,
               { math::random(0, 500), math::random(0, 500) },
               { 50, 50 },
               { math::random(25, 189), math::random(2, 60), math::random(128, 255) } 
            });
            objects.push_back(o);
            connections.insert(std::pair{conn, o});
        }

        try {
            s->send(hdl, "JOIN SUCCESS", websocketpp::frame::opcode::TEXT);
        } catch (const websocketpp::lib::error_code& e) {
            std::cerr << "Echo failed because: " << e
                    << "(" << e.message() << ")" << std::endl;
        }

        std::cout << connections.size() << " active connections now" << std::endl;
    }
    else if (opcode == opcodes::SERVER_EVENT) {
        std::cout << "got SERVER_EVENT opcode" << std::endl;
        // find object associated with the handle
        ACQUIRE_STATE_LOCK;
        auto it = connections.find(conn);
        if (it != connections.end()) {
            uint8_t event = static_cast<uint8_t>(*(data + 1));
            std::cout << "got server event: " << event << std::endl;
            auto obj = it->second;
            switch (event) {
                case server_events::JUMP:
                    obj->vel.y = -100;
                    break;
                case server_events::MOVE_LEFT:
                    obj->pos.x -= 20;
                    break;
                case server_events::MOVE_RIGHT:
                    obj->pos.x += 20;
                    break;
            }
        }
        else {
            std::cerr << "cannot process server event for connection with no associated object" << std::endl;
        }
    }
}

void on_close(websocketpp::connection_hdl hdl) {
    std::cout << "connection closed with hdl: " << hdl.lock().get() << std::endl;

    // find this handle and remove it/its obj from the set of connections
    connection m_conn = ws_server.get_con_from_hdl(hdl);
    {
        ACQUIRE_STATE_LOCK;
        auto it = connections.find(m_conn);
        if (it != connections.end()) {
            auto obj = it->second;
            objects.erase(std::find(objects.begin(), objects.end(), obj));
            connections.erase(it);
        }
    }

    std::cout << connections.size() << " active connections now" << std::endl;
}

void networkLoop(server* s) {
    std::byte* buffer = static_cast<std::byte*>(malloc(10000));
    while (!terminate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            ACQUIRE_STATE_LOCK;
            // serialize game state
            size_t len = 0;
            writeInt(buffer, objects.size(), len);
            for (const auto& obj : objects) {
                serializeObject(*obj.get(), buffer, len);
            }
            for (const auto& [conn, _] : connections) {
                try {
                    s->send(conn, buffer, len, websocketpp::frame::opcode::BINARY);
                }
                catch (...) {
                    std::cerr << "error occurred sending update to client" << std::endl;
                }
            }
        }
    }
    free(buffer);
}

void gameLoop() {
    {
        ACQUIRE_STATE_LOCK;
        std::shared_ptr<object> o = std::make_shared<object>(object{
            ++lastObjectID,
            { math::random(0, 500), math::random(0, 500) },
            { 50, 50 },
            { math::random(25, 189), math::random(2, 60), math::random(128, 255) } 
        });
        objects.push_back(o);
    }

    float gameTime = 0;

    while (!terminate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        // update game.
        float dt = 0.05f;
        gameTime += dt;
        {
            ACQUIRE_STATE_LOCK;
            for (auto& obj : objects) {
                if (obj->id == 1) {
                    obj->pos.x = 250 + std::cosf(gameTime) * 100;
                    obj->pos.y = 250 + std::sinf(gameTime) * 100;
                }
                else {
                    updateObject(*obj.get(), dt);
                }
            }
        }
    }
}

int main() {
    std::thread network_thread(networkLoop, &ws_server);
    std::thread game_thread(gameLoop);

    try {
        ws_server.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload); 

        ws_server.init_asio();
        ws_server.set_reuse_addr(true);

        ws_server.set_message_handler(bind(&on_message, &ws_server, ::_1, ::_2));
        ws_server.set_close_handler(on_close);
        ws_server.listen(9002);
        ws_server.start_accept();

        ws_server.run();
    } catch (websocketpp::exception const & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        terminate = true;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        terminate = true;
    }

    game_thread.join();
    network_thread.join();

    ws_server.stop();

    return 0;
}
