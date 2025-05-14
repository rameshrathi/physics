//
// Created by ramesh on 14/05/25.
//

#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/json.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace asio = boost::asio;           // from <boost/asio.hpp>
namespace ip = boost::asio::ip;         // from <boost/asio/ip.hpp>
namespace json = boost::json;           // from <boost/json.hpp>

// Report a failure
inline void fail(const beast::error_code & ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Callback type for handling received JSON messages
using JsonResponseCallback = std::function<void(beast::error_code, const json::value&)>;

// Represents a single WebSocket client connection
class WSClient : public std::enable_shared_from_this<WSClient>
{
    ip::tcp::resolver resolver_;
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_; // Buffer for incoming data
    std::string host_;
    std::string port_;
    std::string target_;
    JsonResponseCallback callback_;

public:
    // Constructor
    explicit
    WSClient(asio::io_context& ioc)
        : resolver_(ioc)
        , ws_(ioc)
    {
    }

    // Destructor (closes the WebSocket connection)
    ~WSClient()
    {
        // Attempt to close the connection gracefully
        if (ws_.is_open())
        {
            beast::error_code ec;
            ws_.close(websocket::close_code::normal, ec);
            if (ec)
            {
                fail(ec, "close");
            }
        }
    }

    // Stopping copy/move
    WSClient(WSClient const&) = delete;
    WSClient(WSClient&&) = delete;
    WSClient& operator=(WSClient const&) = delete;
    WSClient& operator=(WSClient&&) = delete;

    // Start the asynchronous operation to connect
    void
    connect(
        const std::string& host,
        const std::string& port,
        const std::string& target,
        JsonResponseCallback callback);

    // Send a JSON value as a WebSocket text message
    void
    send(const json::value& msg);

private:
    // Handler for the resolve operation
    void
    on_resolve(
        beast::error_code ec,
        const ip::tcp::resolver::results_type& results);

    // Handler for the connect operation
    void
    on_connect(beast::error_code ec, ip::tcp::resolver::results_type::endpoint_type);

    // Handler for the handshake operation
    void
    on_handshake(beast::error_code ec);

    // Start an asynchronous read operation
    void
    do_read();

    // Handler for the read operation
    void
    on_read(
        beast::error_code ec,
        std::size_t bytes_transferred);

    // Handler for the write operation
    void
    on_write(
        beast::error_code ec,
        std::size_t bytes_transferred);
};