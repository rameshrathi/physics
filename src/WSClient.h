//
// Created by ramesh on 14/05/25.
//

#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json.hpp>
#include <functional>
#include <memory>
#include <string>
#include <chrono>

namespace beast      = boost::beast;
namespace websocket  = beast::websocket;
namespace net        = boost::asio;
namespace ssl        = net::ssl;
namespace json       = boost::json;

using   tcp         = net::ip::tcp;

// JSON parser
class JSONParser final {
public:
    static json::value parse(const std::string& payload) {
        json::value v = json::parse(payload);
        return v;
    }
};

class WSClient : public std::enable_shared_from_this<WSClient> {
public:
    using OnMessageCallback   = std::function<void(const std::string&)>;
    using OnParsedCallback    = std::function<void(const std::string& raw, const json::value& parsed)>;
    using OnConnectedCallback = std::function<void()>;
    using OnErrorCallback     = std::function<void(const std::string&)>;

    WSClient(net::io_context& ioc, ssl::context& ctx);

    // Initiate connection; will retry up to max_retries with retry_delay intervals
    void connect(
        const std::string& url,
        const std::string& port,
        int max_retries = 3,
        std::chrono::milliseconds retry_delay = std::chrono::milliseconds(500)
    );

    // Send a message; retries on failure
    void send(
        const std::string& message,
        int max_retries = 3,
        std::chrono::milliseconds retry_delay = std::chrono::milliseconds(200)
    );

    // Close the WebSocket gracefully
    void close();

    // Set a custom JSON parser for type T
    template <typename T>
    void set_json_parser(std::shared_ptr<JSONParser> parser) {
        json_parser_ = parser;
    }

    // Callback setters
    void set_on_message(OnMessageCallback cb);
    void set_on_parsed(OnParsedCallback cb);
    void set_on_connected(OnConnectedCallback cb);
    void set_on_error(OnErrorCallback cb);

private:
    // Networking members
    tcp::resolver                                   resolver_;
    websocket::stream<beast::ssl_stream<tcp::socket>> ws_;
    beast::flat_buffer                              buffer_;
    std::string                                     host_;
    std::string                                     port_;

    // Retry settings
    int                                              connect_retries_{0};
    int                                              send_retries_{0};
    int                                              max_connect_retries_{3};
    int                                              max_send_retries_{3};
    std::chrono::milliseconds                        connect_delay_;
    std::chrono::milliseconds                        send_delay_;

    // Callbacks
    OnMessageCallback     on_message_;
    OnParsedCallback      on_parsed_;
    OnConnectedCallback   on_connected_;
    OnErrorCallback       on_error_;

    // Generic holder for JSONParser<T>
    std::shared_ptr<void> json_parser_;

    // Internal steps
    void do_resolve();
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type endpoint);
    void on_ssl_handshake(beast::error_code ec);
    void on_ws_handshake(beast::error_code ec);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_close(beast::error_code ec);
    void fail(const beast::error_code& ec, const char* stage) const;
};