#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "WSClient.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
namespace json = boost::json;

// Example usage
int main()
{
    // The io_context is required for all I/O
    asio::io_context ioc;

    // Create multiple clients for different endpoints
    auto client1 = std::make_shared<WSClient>(ioc);
    auto client2 = std::make_shared<WSClient>(ioc);

    // Connect client1 to an echo server (replace with your actual endpoint)
    client1->connect(
        "echo.websocket.org", // Host
        "80",                 // Port (use "443" for wss://)
        "/",                  // Target path
        [](beast::error_code ec, const json::value& received_json)
        {
            if (!ec)
            {
                std::cout << "Client 1 received JSON: " << received_json << std::endl;
                // Process the received JSON here
                if (received_json.is_object()) {
                    // Example: Access a field
                    if (received_json.as_object().count("message")) {
                         std::cout << "  Message field: " << received_json.as_object().at("message").as_string() << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "Client 1 error in callback: " << ec.message() << std::endl;
            }
        });

    // Connect client2 to another endpoint (replace with your actual endpoint)
     client2->connect(
        "echo.websocket.org", // Host
        "80",                 // Port
        "/some/other/path",   // Different target path
        [](beast::error_code ec, const json::value& received_json)
        {
            if (!ec)
            {
                std::cout << "Client 2 received JSON: " << received_json << std::endl;
                 if (received_json.is_object()) {
                    // Example: Access a field
                    if (received_json.as_object().count("data")) {
                         std::cout << "  Data field: " << received_json.as_object().at("data") << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "Client 2 error in callback: " << ec.message() << std::endl;
            }
        });


    // Give some time for connections to establish before sending
    asio::steady_timer timer(ioc, std::chrono::seconds(2));
    timer.async_wait([&](const beast::error_code& ec) {
        if (!ec) {
            // Send a JSON message from client1
            json::value msg1 = {{"command", "hello"}, {"user", "client1"}};
            client1->send(msg1);

             // Send a JSON message from client2
            const json::value msg2 = {{"type", "request"}, {"data", 123}};
            client2->send(msg2);
        } else {
             std::cerr << "Timer error: " << ec.message() << std::endl;
        }
    });


    // Run the I/O service. The call will return when there are no more
    // asynchronous operations pending.
    std::cout << "Starting io_context run..." << std::endl;
    ioc.run();
    std::cout << "io_context run finished." << std::endl;

    return 0;
}