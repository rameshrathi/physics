// main.cpp

#include "WSClient.h"
#include <boost/json.hpp>
#include <boost/json/value.hpp>
#include <iostream>
#include <vector>

namespace  json = boost::json;

// Configuration Module
struct Config {
    std::string exchange = "OKX";
    std::string spotAsset = "BTC-USDT-SWAP";
    std::string orderType = "market";
    double quantity = 100.0; // USD equivalent
    double volatility = 0.02; // Example value
    int feeTier = 0; // Example tier
};

int main() {
    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    const auto client = std::make_shared<WSClient>(ioc, ctx);

    // Callbacks
    client->set_on_connected([]{
        std::cout << "[Connected]" << std::endl;
    });

    client->set_on_message([](const std::string& raw){
        std::cout << "[Raw  incoming] " << raw << std::endl;
    });

    const stock::TickerParser parser;
    client->set_parser<stock::Ticker>(parser, [](const stock::Ticker& ticker) {
        std::cout << "[Parsed outgoing] " << ticker.symbol << " " << ticker.asks.size() << std::endl;
    });

    client->set_on_error([](const std::string& err){
        std::cerr << "[Error] " << err << std::endl;
    });

    // Connect to
    client->connect("localhost", "8765");

    // Once connected, send a JSON message
    client->set_on_connected([&]{
        json::value msg = {
            {"type", "greeting"},
            {"payload", "Hello, secure echo!"}
        };
        client->send("Hello From Client!");
    });

    ioc.run();
    return 0;
}