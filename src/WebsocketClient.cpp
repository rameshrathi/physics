//
// Created by ramesh on 14/05/25.
//

#include <boost/beast/core.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/json.hpp>

#include "WebsocketClient.h"

// Start the asynchronous operation to connect
void
WebsocketClient::connect(
    const std::string& host,
    const std::string& port,
    const std::string& target,
    JsonResponseCallback callback)
{
    host_ = host;
    port_ = port;
    target_ = target;
    callback_ = std::move(callback);

    // Look up the domain name
    resolver_.async_resolve(
        host_,
        port_,
        beast::bind_front_handler(
            &WebsocketClient::on_resolve,
            shared_from_this())); // Use shared_from_this for handler lifetime
}

// Send a JSON value as a WebSocket text message
void
WebsocketClient::send(const json::value& msg)
{
    // Serialize the JSON value to a string
    std::string ss = json::serialize(msg);

    // Write the string asynchronously
    ws_.async_write(
        asio::buffer(ss),
        beast::bind_front_handler(
            &WebsocketClient::on_write,
            shared_from_this())); // Use shared_from_this for handler lifetime
}

// Handler for the resolve operation
void
WebsocketClient::on_resolve(
    beast::error_code ec,
    const ip::tcp::resolver::results_type& results)
{
    if(ec)
        return fail(ec, "resolve");

    // Set a timeout for the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(
            &WebsocketClient::on_connect,
            shared_from_this())); // Use shared_from_this for handler lifetime
}

// Handler for the connect operation
void
WebsocketClient::on_connect(beast::error_code ec, ip::tcp::resolver::results_type::endpoint_type)
{
    if(ec)
        return fail(ec, "connect");

    // Perform the websocket handshake
    ws_.async_handshake(
        host_,
        target_,
        beast::bind_front_handler(
            &WebsocketClient::on_handshake,
            shared_from_this())); // Use shared_from_this for handler lifetime
}

// Handler for the handshake operation
void
WebsocketClient::on_handshake(beast::error_code ec)
{
    if(ec)
        return fail(ec, "handshake");

    // Handshake is complete, start reading messages
    do_read();
}

// Start an asynchronous read operation
void
WebsocketClient::do_read()
{
    // Read a message into our buffer
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &WebsocketClient::on_read,
            shared_from_this())); // Use shared_from_this for handler lifetime
}

// Handler for the read operation
void
WebsocketClient::on_read(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec == websocket::error::closed)
        return; // Connection was closed gracefully

    if(ec)
        return fail(ec, "read");

    // Check if the received message is text and complete
    if (ws_.got_text() && ws_.is_message_done())
    {
        // Get the received message payload
        beast::string_view received_payload = beast::buffers_to_string(buffer_.data());

        try
        {
            // Parse the received JSON string
            json::value received_json = json::parse(received_payload);

            // Invoke the user-provided callback with the parsed JSON
            if (callback_)
            {
                callback_(beast::error_code{}, received_json);
            }
        }
        catch (const beast::system_error & e)
        {
            // Handle JSON parsing errors
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            // Optionally, report this error via the callback
             if (callback_)
            {
                callback_(e.code(), json::value()); // Pass the JSON error code
            }
        }
        catch (const std::exception& e)
        {
            // Handle other potential errors during processing
            std::cerr << "Processing error: " << e.what() << std::endl;
             if (callback_)
            {
                // Create a generic error code or use a specific one if available
                beast::error_code processing_ec(static_cast<int>(asio::error::operation_aborted), asio::error::get_system_category());
                callback_(processing_ec, json::value());
            }
        }
    }
    else
    {
        // Handle non-text messages or fragmented messages if needed
        std::cerr << "Received non-text or fragmented message." << std::endl;
        // Depending on requirements, you might ignore, log, or handle differently.
        // For this example, we'll just log and continue reading.
    }


    // Clear the buffer for the next read
    buffer_.consume(buffer_.size());

    // Continue reading messages
    do_read();
}

// Handler for the write operation
void
WebsocketClient::on_write(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "write");

    // Write is complete. The read loop is already running.
    // If you had a request/response pattern where you send one message
    // and expect one response, you might manage state here.
    // For a continuous stream, just let do_read handle incoming.
}