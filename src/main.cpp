// main.cpp

#include "WSClient.h"
#include <boost/json.hpp>
#include <boost/json/value.hpp>
#include <iostream>

namespace  json = boost::json;

namespace stock {
    // Example stock ticker
    struct Ticker {
        std::string symbol;
        double price{};
    };
    // Example stock ticker parser
    struct BuildTicker final {
        static Ticker build(const json::value& json) {
            Ticker tk;
            tk.symbol = json.at("symbol").as_string();
            return tk;
        }
    };
} // stock

int main() {
    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    const auto client = std::make_shared<WSClient>(ioc, ctx);

    // Install JSON parser
    const auto parser = std::make_shared<JSONParser>();
    client->set_json_parser<JSONParser>(parser);

    // Callbacks
    client->set_on_connected([]{
        std::cout << "[Connected]" << std::endl;
    });

    client->set_on_message([](const std::string& raw){
        std::cout << "[Raw  incoming] " << raw << std::endl;
    });

    client->set_on_parsed([](const std::string& raw, const json::value& parsed){
        std::cout << "[Parsed incoming] " << parsed << std::endl;
    });

    client->set_on_error([](const std::string& err){
        std::cerr << "[Error] " << err << std::endl;
    });

    // Connect to
    client->connect("echo.websocket.events", "443");

    // Once connected, send a JSON message
    client->set_on_connected([&]{
        json::value msg = {
            {"type", "greeting"},
            {"payload", "Hello, secure echo!"}
        };
        client->send("msg");
    });

    ioc.run();
    return 0;
}