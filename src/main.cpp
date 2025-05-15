// main.cpp

#include <boost/json.hpp>
#include <iostream>

#include "WSClient.h"
#include "CalculationEngine.h"
#include "OrderBook.h"
#include "TradingView.h"

namespace  json = boost::json;

int main() {
    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    const auto client = std::make_shared<WSClient>(ioc, ctx);

    Config config;
    stock::Orderbook ob;
    CalculationEngine calc(config);
    TradingView ui(config, calc);
    ui.setup();

    // Callbacks
    client->set_on_connected([]{
        std::cout << "[Connected]" << std::endl;
    });

    client->set_on_message([](const std::string& raw){
        std::cout << "[Raw  incoming] " << raw << std::endl;
    });

    const stock::OrderbookParser parser;
    client->set_parser<stock::Orderbook>(parser, [&ob, &calc, &ui](const stock::Orderbook& data) {
        std::cout << "[Parsed outgoing] " << ob.getSymbol() << " " << ob.getTimestamp() << std::endl;
        stock::Orderbook tmp = data;
        ob.updateOrderbook(std::move(tmp));
        calc.processTick(ob);
        ui.render();
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


    // std::thread wsThread([&ws, &ob, &calc]() {
    //     ws.onMessage([&ob, &calc](const json& data) {
    //         ob.updateOrderbook(data);
    //         calc.processTick(ob);
    //     });
    //     ws.connect("wss://ws.gomarket-cpp.goquant.io/ws/l2-orderbook/okx/BTC-USDT-SWAP");
    // });
    //
    // // Main loop (simplified, assumes ImGui setup)
    // while (true) {
    //     ui.render();
    //     std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    // }
    //
    // wsThread.join();


    ioc.run();
    return 0;
}