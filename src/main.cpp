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
    struct TickerParser final : IParser<Ticker> {
        Ticker operator()(const std::string& msg) const override {
            Ticker ticker;
            json::value parsed = json::parse(msg);
            ticker.symbol = parsed.at("symbol").as_string();
            ticker.price = parsed.at("price").as_double();
            return ticker;
        }
    };
} // stock

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
        std::cout << "[Parsed outgoing] " << ticker.symbol << " " << ticker.price << std::endl;
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
        client->send("msg");
    });

    ioc.run();
    return 0;
}