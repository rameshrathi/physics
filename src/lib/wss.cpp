#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace json = boost::json;           // from <boost/json.hpp>

/// Represents a chat message (C++ structure)
struct Message {
    std::string sender;
    std::string content;
    std::chrono::system_clock::time_point timestamp;

    // Helper to convert Message to Boost.Json value
    json::value to_json() const {
        json::object obj;
        obj["sender"] = sender;
        obj["content"] = content;
        // Convert timestamp to ISO 8601 string
        std::time_t tt = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm = *std::gmtime(&tt); // Use gmtime for UTC
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601 format
        obj["timestamp"] = ss.str();
        return obj;
    }

    // Helper to create Message from Boost.Json value
    static beast::expected<Message, beast::error_code> from_json(const json::value& jv) {
        if (!jv.is_object()) {
            return beast::unexpected(beast::errc::invalid_argument);
        }
        const json::object& obj = jv.as_object();

        auto sender_it = obj.find("sender");
        auto content_it = obj.find("content");
        auto timestamp_it = obj.find("timestamp");

        if (sender_it == obj.end() || !sender_it->value().is_string() ||
            content_it == obj.end() || !content_it->value().is_string() ||
            timestamp_it == obj.end() || !timestamp_it->value().is_string()) {
            return beast::unexpected(beast::errc::invalid_argument);
        }

        std::string sender = sender_it->value().as_string().c_str();
        std::string content = content_it->value().as_string().c_str();
        std::string timestamp_str = timestamp_it->value().as_string().c_str();

        // Parse ISO 8601 timestamp string
        std::tm tm = {};
        std::stringstream ss(timestamp_str);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ"); // ISO 8601 format

        if (ss.fail()) {
            // Handle potential failure in parsing timestamp
             return beast::unexpected(beast::errc::invalid_argument);
        }

        std::time_t tt = mktime(&tm);
        if (tt == -1) {
             // Handle potential failure in mktime
             return beast::unexpected(beast::errc::invalid_argument);
        }

        std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::from_time_t(tt);

        return Message{sender, content, timestamp};
    }
};


// MARK: - REST API Client

/// Client for interacting with an external REST API.
class RestApiClient {
private:
    std::string host_;
    int port_;
    std::unique_ptr<httplib::Client> client_;

public:
    RestApiClient(const std::string& host, int port) : host_(host), port_(port) {
         // Use HTTPS client if port is 443 or host indicates secure
        if (port_ == 443 || host_.rfind("https://", 0) == 0) {
             // Remove https:// prefix if present
            if (host_.rfind("https://", 0) == 0) {
                host_ = host_.substr(8);
            }
            client_ = std::make_unique<httplib::Client>(host_, port_);
            client_->enable_server_certificate_verification(false); // WARNING: Disable for testing only!
                                                                    // In production, verify certificates.
        } else {
             // Remove http:// prefix if present
            if (host_.rfind("http://", 0) == 0) {
                host_ = host_.substr(7);
            }
            client_ = std::make_unique<httplib::Client>(host_, port_);
        }

        // Set a timeout for the connection and request
        client_->set_connection_timeout(std::chrono::seconds(5));
        client_->set_read_timeout(std::chrono::seconds(10));
        client_->set_write_timeout(std::chrono::seconds(5));
    }

    /// Performs a GET request to the specified path.
    /// Returns the response body or an error string.
    beast::expected<std::string, std::string> get(const std::string& path) {
        if (!client_) {
            return beast::unexpected("REST client not initialized.");
        }

        auto res = client_->Get(path.c_str());

        if (res) {
            if (res->status == 200) {
                return res->body;
            } else {
                return beast::unexpected("HTTP GET request failed with status: " + std::to_string(res->status));
            }
        } else {
            return beast::unexpected("HTTP GET request failed: " + httplib::to_string(res.error()));
        }
    }

    // Add more methods for POST, PUT, etc. as needed
};

// MARK: - Chat Message Handler

/// Handles the logic for processing chat messages and interacting with the REST API.
class ChatMessageHandler {
private:
    RestApiClient rest_api_client_;

public:
    ChatMessageHandler(const std::string& rest_api_host, int rest_api_port)
        : rest_api_client_(rest_api_host, rest_api_port) {}

    /// Processes an incoming message and generates a response message.
    /// This is where your application-specific logic goes.
    /// For demonstration, it echoes the message content and queries a dummy API.
    Message handle_message(const Message& incoming_message) {
        std::cout << "Handling message from " << incoming_message.sender
                  << ": " << incoming_message.content << std::endl;

        // Example logic: If the message contains "weather", query a weather API.
        // Otherwise, just echo the message content.
        std::string response_content;
        std::string response_sender = "Server";

        if (incoming_message.content.find("weather") != std::string::npos) {
            std::cout << "Querying weather API..." << std::endl;
            // Example REST API call (replace with your actual API endpoint)
            auto api_response = rest_api_client_.get("/weather?city=London"); // Dummy endpoint

            if (api_response) {
                response_content = "Weather info: " + *api_response;
            } else {
                response_content = "Could not get weather info: " + api_response.error();
            }
            response_sender = "Weather Bot";

        } else if (incoming_message.content.find("echo") != std::string::npos) {
             std::cout << "Echoing message..." << std::endl;
             response_content = "Echo: " + incoming_message.content;
             response_sender = "Echo Bot";
        }
        else {
            // Default response: Acknowledge the message
            response_content = "Received your message: '" + incoming_message.content + "'";
            response_sender = "Server";
        }

        // Create the response message
        return Message{response_sender, response_content, std::chrono::system_clock::now()};
    }
};

// MARK: - WebSocket Session

/// Represents a single WebSocket connection.
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    beast::ssl_stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::shared_ptr<ChatMessageHandler> message_handler_; // Shared handler instance

public:
    // Constructor
    WebSocketSession(tcp::socket&& socket, ssl::context& ctx, std::shared_ptr<ChatMessageHandler> handler)
        : ws_(std::move(socket), ctx), message_handler_(handler) {}

    // Start the session by performing the SSL handshake
    void run() {
        net::dispatch(ws_.get_executor(),
                      beast::bind_front_handler(&WebSocketSession::on_run,
                                                shared_from_this()));
    }

private:
    void on_run() {
        // Set a timeout on the SSL handshake
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Perform the SSL handshake
        ws_.async_handshake(ssl::stream_base::server,
                            beast::bind_front_handler(&WebSocketSession::on_ssl_handshake,
                                                      shared_from_this()));
    }

    void on_ssl_handshake(beast::error_code ec) {
        if (ec) {
            std::cerr << "SSL Handshake Error: " << ec.message() << std::endl;
            return fail(ec, "ssl_handshake");
        }

        // Reset the timeout
        beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

        // Set a decorator to change the Server header of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res) {
                res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-cpp");
            }));

        // Accept the websocket handshake
        ws_.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept,
                                                  shared_from_this()));
    }

    void on_accept(beast::error_code ec) {
        if (ec) {
            std::cerr << "WebSocket Accept Error: " << ec.message() << std::endl;
            return fail(ec, "accept");
        }

        // Read a message
        do_read();
    }

    void do_read() {
        // Read a message into our buffer
        ws_.async_read(buffer_,
                       beast::bind_front_handler(&WebSocketSession::on_read,
                                                 shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == websocket::error::closed) {
             std::cout << "WebSocket connection closed by peer." << std::endl;
             return; // Connection closed cleanly
        }

        if (ec) {
            std::cerr << "WebSocket Read Error: " << ec.message() << std::endl;
            return fail(ec, "read");
        }

        // Check if the message is text
        if (ws_.got_text()) {
            std::string received_text = beast::buffers_to_string(buffer_.data());
            std::cout << "Received message: " << received_text << std::endl;

            // Parse the received JSON string
            beast::error_code parse_ec;
            json::value received_json = json::parse(received_text, parse_ec);

            if (parse_ec) {
                std::cerr << "JSON Parse Error: " << parse_ec.message() << std::endl;
                // Send an error message back to the client
                send_error_message("Invalid JSON format.");
            } else {
                // Attempt to create a Message object from JSON
                auto message_result = Message::from_json(received_json);

                if (!message_result) {
                    std::cerr << "Message Decoding Error: " << message_result.error().message() << std::endl;
                    // Send an error message back to the client
                    send_error_message("Invalid message format.");
                } else {
                    // Process the message using the handler
                    Message incoming_message = *message_result;
                    Message response_message = message_handler_->handle_message(incoming_message);

                    // Convert the response message back to JSON
                    json::value response_json = response_message.to_json();
                    std::string response_text = json::serialize(response_json);

                    // Send the response back
                    do_write(response_text);
                }
            }
        } else {
             // Handle non-text messages if necessary (e.g., binary)
             std::cerr << "Received non-text message, ignoring." << std::endl;
             send_error_message("Server expects text messages.");
        }

        // Clear the buffer
        buffer_.consume(buffer_.size());

        // Read the next message
        do_read();
    }

    void do_write(const std::string& text) {
        // Send the message
        ws_.async_write(net::buffer(text),
                        beast::bind_front_handler(&WebSocketSession::on_write,
                                                  shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            std::cerr << "WebSocket Write Error: " << ec.message() << std::endl;
            return fail(ec, "write");
        }

        // Our buffer is empty so we can clear it.
        buffer_.consume(buffer_.size());

        // Read a new message
        do_read();
    }

    // Helper function to send an error message back to the client
    void send_error_message(const std::string& error_description) {
        Message error_msg{"Server Error", error_description, std::chrono::system_clock::now()};
        json::value error_json = error_msg.to_json();
        std::string error_text = json::serialize(error_json);
        do_write(error_text);
    }

    void fail(beast::error_code ec, char const* what) {
        // Don't report these errors reported too often
        if (ec == net::ssl::error::stream_truncated ||
            ec == websocket::error::closed)
            return;

        std::cerr << what << ": " << ec.message() << "\n";
    }
};

// MARK: - WebSocket Server

/// Accepts incoming WebSocket connections.
class WebSocketServer {
    net::io_context& ioc_;
    ssl::context ctx_;
    tcp::acceptor acceptor_;
    std::shared_ptr<ChatMessageHandler> message_handler_; // Shared handler instance

public:
    // Constructor
    WebSocketServer(net::io_context& ioc, unsigned short port,
                    const std::string& cert_path, const std::string& key_path,
                    const std::string& rest_api_host, int rest_api_port)
        : ioc_(ioc),
          ctx_(ssl::context::tlsv12_server), // Use TLSv1.2 or higher
          acceptor_(ioc, {tcp::v4(), port}),
          message_handler_(std::make_shared<ChatMessageHandler>(rest_api_host, rest_api_port))
    {
        // Load SSL certificates
        load_server_certificates(ctx_, cert_path, key_path);

        // Start accepting connections
        do_accept();
    }

private:
    // Load SSL certificates
    void load_server_certificates(ssl::context& ctx, const std::string& cert_path, const std::string& key_path) {
        try {
            ctx.use_certificate_chain_file(cert_path);
            ctx.use_private_key_file(key_path, ssl::context::private_key_file_format::pem);
            std::cout << "SSL certificates loaded successfully." << std::endl;
        } catch (const boost::system::system_error& e) {
            std::cerr << "Error loading SSL certificates: " << e.what() << std::endl;
            // In a real application, you would handle this more gracefully,
            // perhaps exiting or throwing an exception.
            throw std::runtime_error("Failed to load SSL certificates.");
        }
    }

    // Start accepting new connections
    void do_accept() {
        acceptor_.async_accept(net::make_strand(ioc_),
                               beast::bind_front_handler(&WebSocketServer::on_accept,
                                                         this));
    }

    // Handle a new incoming connection
    void on_accept(beast::error_code ec, tcp::socket socket) {
        if (ec) {
            std::cerr << "Accept Error: " << ec.message() << std::endl;
        } else {
            // Create the session and run it
            std::make_shared<WebSocketSession>(std::move(socket), ctx_, message_handler_)->run();
        }

        // Accept the next connection
        do_accept();
    }
};

// MARK: - Main Function

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 6) {
        std::cerr <<
            "Usage: websocket-server-cpp <address> <port> <cert_file> <key_file> <rest_api_url>\n" <<
            "Example:\n" <<
            "    websocket-server-cpp 0.0.0.0 8080 server.pem server.key http://api.example.com:5000\n";
        return EXIT_FAILURE;
    }

    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const cert_path = argv[3];
    auto const key_path = argv[4];
    auto const rest_api_url_str = argv[5];

    // Parse the REST API URL
    httplib::Result rest_api_url_parsed = httplib::parse_url(rest_api_url_str);
    if (!rest_api_url_parsed) {
        std::cerr << "Error parsing REST API URL: " << rest_api_url_str << std::endl;
        return EXIT_FAILURE;
    }

    std::string rest_api_host = rest_api_url_parsed->domain;
    int rest_api_port = rest_api_url_parsed->port;
    bool rest_api_is_https = (rest_api_url_parsed->scheme == "https");

    // Default port based on scheme if not specified
    if (rest_api_port == 0) {
        rest_api_port = rest_api_is_https ? 443 : 80;
    }


    // The io_context is required for all I/O
    net::io_context ioc{1}; // Use 1 thread for the IO context for simplicity

    // Create and run the server
    try {
        WebSocketServer server{ioc, port, cert_path, key_path, rest_api_host, rest_api_port};

        // Capture SIGINT and SIGTERM to allow graceful shutdown
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&](const beast::error_code&, int) {
            // Stop the io_context
            ioc.stop();
        });

        std::cout << "WebSocket server started on port " << port << " (WSS)" << std::endl;
        std::cout << "Connecting to REST API at " << (rest_api_is_https ? "https://" : "http://") << rest_api_host << ":" << rest_api_port << std::endl;


        // Run the I/O context. This will block until the context has no more work.
        ioc.run();

    } catch (const std::exception& e) {
        std::cerr << "Server startup error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    std::cout << "Server stopped." << std::endl;

    return EXIT_SUCCESS;
}

