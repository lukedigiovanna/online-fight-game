#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_THREAD_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <memory>
#include <algorithm>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using connection = std::shared_ptr<websocketpp::connection<websocketpp::config::asio>>;

std::atomic<bool> terminate;
std::vector<connection> connections;
server ws_server;

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    std::string payload = msg->get_payload();

    if (payload == "join") {
        std::shared_ptr conn = s->get_con_from_hdl(hdl);
        connections.push_back(conn);
        try {
            s->send(hdl, "JOIN SUCCESS", websocketpp::frame::opcode::TEXT);
        } catch (const websocketpp::lib::error_code& e) {
            std::cerr << "Echo failed because: " << e
                    << "(" << e.message() << ")" << std::endl;
        }

        std::cout << connections.size() << " active connections now" << std::endl;
    }
}

void on_close(websocketpp::connection_hdl hdl) {
    std::cout << "connection closed with hdl: " << hdl.lock().get() << std::endl;

    // find this handle and remove it from the set of connections
    connection conn = ws_server.get_con_from_hdl(hdl);
    connections.erase(std::find(connections.begin(), connections.end(), conn));

    std::cout << connections.size() << " active connections now" << std::endl;
}

void gameLoop(server* s) {
    while (!terminate) {
        std::cout << "gamer\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for (const connection conn : connections) {
            try {
                s->send(conn, "gamer", websocketpp::frame::opcode::TEXT);
            }
            catch (...) {
                std::cerr << "error occurred sending update to client" << std::endl;
            }
        }
    }
}

int main() {
    std::thread game_thread(gameLoop, &ws_server);;

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

    ws_server.stop();

    return 0;
}
