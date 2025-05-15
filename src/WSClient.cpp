//
// Created by ramesh on 14/05/25.
//

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include "WSClient.h"

namespace  json = boost::json;

WSClient::WSClient(net::io_context& ioc, ssl::context& ctx)
    : resolver_(net::make_strand(ioc)),
    ws_(net::make_strand(ioc), ctx),
    connect_delay_(0),
    send_delay_(0)
    {}

void WSClient::connect(
    const std::string& url,
    const std::string& port,
    const int max_retries,
    std::chrono::milliseconds retry_delay
) {
    host_ = url;
    port_ = port;
    max_connect_retries_ = max_retries;
    connect_delay_ = retry_delay;
    connect_retries_ = 0;
    do_resolve();
}

void WSClient::do_resolve() {
    resolver_.async_resolve(host_, port_,
        beast::bind_front_handler(&WSClient::on_resolve, shared_from_this())
        );
}

void WSClient::on_resolve(beast::error_code ec, const tcp::resolver::results_type& results) {
    if (ec) {
        if (connect_retries_++ < max_connect_retries_) {
            std::this_thread::sleep_for(connect_delay_);
            do_resolve();
            return;
        }
        return fail(ec, "resolve");
    }
    net::async_connect(
        ws_.next_layer().next_layer(),
        results,
        beast::bind_front_handler(&WSClient::on_connect, shared_from_this())
        );
}

void WSClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
    if (ec) {
        if (connect_retries_++ < max_connect_retries_) {
            std::this_thread::sleep_for(connect_delay_);
            do_resolve();
            return;
        }
        return fail(ec, "connect");
    }
    ws_.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(&WSClient::on_ssl_handshake, shared_from_this()));
}

void WSClient::on_ssl_handshake(beast::error_code ec) {
    if (ec) return fail(ec, "ssl_handshake");
    ws_.async_handshake(host_, "/",
        beast::bind_front_handler(&WSClient::on_ws_handshake, shared_from_this())
        );
}

void WSClient::on_ws_handshake(beast::error_code ec) {
    if (ec) return fail(ec, "websocket_handshake");
    if (on_connected_) on_connected_();
    ws_.async_read(buffer_,
        beast::bind_front_handler(&WSClient::on_read, shared_from_this())
        );
}

void WSClient::send(
    const std::string& message,
    const int max_retries,
    std::chrono::milliseconds retry_delay)
{
    max_send_retries_ = max_retries;
    send_delay_ = retry_delay;
    send_retries_ = 0;
    ws_.async_write(
        net::buffer(message),
        beast::bind_front_handler(&WSClient::on_write, shared_from_this())
        );
}

void WSClient::on_write(beast::error_code ec, std::size_t) {
    if (ec) {
        if (send_retries_++ < max_send_retries_) {
            std::this_thread::sleep_for(send_delay_);
            ws_.async_write(
                net::buffer(buffer_.data()),
                beast::bind_front_handler(&WSClient::on_write, shared_from_this())
                );
            return;
        }
        return fail(ec, "write");
    }
    buffer_.consume(buffer_.size());
        ws_.async_read(buffer_,
        beast::bind_front_handler(&WSClient::on_read, shared_from_this())
        );
}

void WSClient::on_read(beast::error_code ec, std::size_t) {
    if (ec)
        return fail(ec, "read");

    const std::string raw = beast::buffers_to_string(buffer_.data());
    if (on_message_)
        on_message_(raw);

    // Attempt parsing if parser provided
    if (parser_handler_) {
        parser_handler_->parseAndCallback(raw);
    }
    buffer_.consume(buffer_.size());
    ws_.async_read(buffer_,
        beast::bind_front_handler(&WSClient::on_read, shared_from_this())
        );
}

void WSClient::close() {
    ws_.async_close(websocket::close_code::normal,
        beast::bind_front_handler(&WSClient::on_close, shared_from_this())
        );
}

void WSClient::on_close(beast::error_code ec) const {
    if (ec) fail(ec, "close");
}

void WSClient::set_on_message(OnMessageCallback cb)   { on_message_   = std::move(cb); }
void WSClient::set_on_connected(OnConnectedCallback cb) { on_connected_ = std::move(cb); }
void WSClient::set_on_error(OnErrorCallback cb)       { on_error_     = std::move(cb); }

void WSClient::fail(const beast::error_code& ec, const char* stage) const {
    std::string msg = stage;
    if (ec) msg += std::string(": ") + ec.message();
    if (on_error_) on_error_(msg);
    else std::cerr << msg << std::endl;
}
