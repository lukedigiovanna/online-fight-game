#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_THREAD_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

std::atomic<bool> terminate;
std::vector<websocketpp::connection_hdl> connections;

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;

    std::string payload = msg->get_payload();

    if (payload == "join") {
        connections.push_back(hdl);
        try {
            s->send(hdl, "JOIN SUCCESS", websocketpp::frame::opcode::TEXT);
        } catch (const websocketpp::lib::error_code& e) {
            std::cerr << "Echo failed because: " << e
                    << "(" << e.message() << ")" << std::endl;
        }
    }
}

void gameLoop(server* s) {
    while (!terminate) {
        std::cout << "gamer\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for (const websocketpp::connection_hdl conn : connections) {
            s->send(conn, "gamer", websocketpp::frame::opcode::TEXT);
        }
    }
}

int main() {
    server serv;
    std::thread game_thread(gameLoop, &serv);;

    try {
        serv.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload); 

        serv.init_asio();
        serv.set_message_handler(bind(&on_message, &serv, ::_1, ::_2));
        serv.listen(9002);
        serv.start_accept();

        serv.run();
    } catch (websocketpp::exception const & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        terminate = true;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        terminate = true;
    }

    game_thread.join();

    return 0;
}
