#include "chat.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <thread>
#include <sstream>
#include <algorithm>

// Constructor
ChatService::ChatService(unsigned int port, const std::string& auth_token)
    : port(port), auth_token(auth_token), running(false), server_socket(-1) {}

// Destructor
ChatService::~ChatService() {
    stop();
}

// Start the chat service
bool ChatService::start() {
    if (running.load()) {
        std::cerr << "Chat service is already running" << std::endl;
        return false;
    }

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    // Set socket options to allow reuse of address/port
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        close(server_socket);
        return false;
    }

    // Configure server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(server_socket);
        return false;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(server_socket);
        return false;
    }

    // Set the running flag
    running.store(true);

    // Start the server thread
    server_thread = std::thread(&ChatService::server_loop, this);

    std::cout << "Chat service started on port " << port << std::endl;
    return true;
}

// Stop the chat service
void ChatService::stop() {
    if (!running.load()) {
        return;
    }

    // Set the flag to stop the server loop
    running.store(false);

    // Close the server socket to unblock accept()
    if (server_socket != -1) {
        close(server_socket);
        server_socket = -1;
    }

    // Close all client sockets
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (auto& client : clients) {
            close(client.socket);
        }
        clients.clear();
    }

    // Wait for the server thread to finish
    if (server_thread.joinable()) {
        server_thread.join();
    }

    std::cout << "Chat service stopped" << std::endl;
}

// Check if the service is running
bool ChatService::is_running() const {
    return running.load();
}

// Get the port
unsigned int ChatService::get_port() const {
    return port;
}

// Get the client count
unsigned int ChatService::get_client_count() const {
    // std::unique_lock<std::mutex> lock(clients_mutex);
    return clients.size();
}

// Main server loop
void ChatService::server_loop() {
    std::cout << "Chat server started. Waiting for connections..." << std::endl;

    // Set the server socket to non-blocking mode
    int flags = fcntl(server_socket, F_GETFL, 0);
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

    while (running.load()) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_socket < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No pending connections, sleep for a short time
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            } else if (!running.load()) {
                // Server is shutting down
                break;
            } else {
                std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                continue;
            }
        }

        // Get client IP address
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "New client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Create a new thread to handle the client
        std::thread client_thread(&ChatService::handle_client, this, client_socket);
        client_thread.detach();
    }

    std::cout << "Chat server loop ended" << std::endl;
}

// Handle a client
void ChatService::handle_client(int client_socket) {
    // Send welcome message
    std::string welcome_msg = "Welcome to the Secure Chat Server!\nPlease enter the authentication token: ";
    send(client_socket, welcome_msg.c_str(), welcome_msg.length(), 0);

    // Authenticate the client
    if (!authenticate_client(client_socket)) {
        close(client_socket);
        return;
    }

    // Get username
    std::string request_username = "Authentication successful. Please enter your username: ";
    send(client_socket, request_username.c_str(), request_username.length(), 0);

    char username_buf[64] = {0};
    ssize_t username_len = recv(client_socket, username_buf, sizeof(username_buf) - 1, 0);
    if (username_len <= 0) {
        std::cerr << "Failed to receive username" << std::endl;
        close(client_socket);
        return;
    }

    // Trim username
    std::string username(username_buf);
    username.erase(std::remove(username.begin(), username.end(), '\n'), username.end());
    username.erase(std::remove(username.begin(), username.end(), '\r'), username.end());

    // Add client to the list
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back({client_socket, username, true});
    }

    // Send welcome message to all clients
    std::string join_msg = username + " has joined the chat!\n";
    broadcast_message(join_msg, client_socket);

    // Send welcome message to the client
    std::string instructions = "You are now connected to the chat server. Type your messages and press Enter to send.\n";
    send(client_socket, instructions.c_str(), instructions.length(), 0);

    // Handle client messages
    char buffer[1024];
    ssize_t bytes_received;

    while (running.load()) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            // Client disconnected or error
            break;
        }

        // Prepare the message with the username
        std::string message(buffer);
        message = username + ": " + message;

        // Broadcast the message to all clients
        broadcast_message(message, client_socket);
    }

    // Client disconnected
    std::string leave_msg = username + " has left the chat.\n";
    broadcast_message(leave_msg, client_socket);

    // Remove client from the list
    remove_client(client_socket);
}

// Authenticate a client
bool ChatService::authenticate_client(int client_socket) {
    char auth_buf[256] = {0};
    ssize_t auth_len = recv(client_socket, auth_buf, sizeof(auth_buf) - 1, 0);
    
    if (auth_len <= 0) {
        std::cerr << "Failed to receive authentication token" << std::endl;
        return false;
    }

    // Trim the token (remove newline characters)
    std::string received_token(auth_buf);
    received_token.erase(std::remove(received_token.begin(), received_token.end(), '\n'), received_token.end());
    received_token.erase(std::remove(received_token.begin(), received_token.end(), '\r'), received_token.end());

    // Check if the token is valid
    if (received_token != auth_token) {
        std::string auth_fail = "Authentication failed. Invalid token.\n";
        send(client_socket, auth_fail.c_str(), auth_fail.length(), 0);
        std::cerr << "Authentication failed for a client" << std::endl;
        return false;
    }

    return true;
}

// Broadcast a message to all clients
void ChatService::broadcast_message(const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    for (const auto& client : clients) {
        // Don't send the message back to the sender
        if (client.socket != sender_socket) {
            send(client.socket, message.c_str(), message.length(), 0);
        }
    }
}

// Remove a client from the list
void ChatService::remove_client(int client_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    clients.erase(
        std::remove_if(
            clients.begin(), 
            clients.end(),
            [client_socket](const ChatClient& client) {
                return client.socket == client_socket;
            }
        ),
        clients.end()
    );

    close(client_socket);
}

// Display the chat service menu
void ChatService::display_menu() {
    std::cout << "\n======== Chat Service ========\n" << std::endl;
    std::cout << "1. Start Chat Server" << std::endl;
    std::cout << "2. Stop Chat Server" << std::endl;
    std::cout << "3. Show Chat Server Status" << std::endl;
    std::cout << "4. Return to Main Menu" << std::endl;
    std::cout << "\nEnter your choice (1-4): ";
}

// Handle menu choice for chat service
void ChatService::handle_menu_choice() {
    static ChatService chat_service;
    
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1: {
            if (!chat_service.is_running()) {
                std::cout << "Starting chat server..." << std::endl;
                if (chat_service.start()) {
                    std::cout << "Chat server started on port " << chat_service.get_port() << std::endl;
                    std::cout << "Use the token '" << "secure_token" << "' to authenticate." << std::endl;
                } else {
                    std::cerr << "Failed to start chat server" << std::endl;
                }
            } else {
                std::cout << "Chat server is already running on port " << chat_service.get_port() << std::endl;
            }
            break;
        }
        case 2: {
            if (chat_service.is_running()) {
                std::cout << "Stopping chat server..." << std::endl;
                chat_service.stop();
                std::cout << "Chat server stopped" << std::endl;
            } else {
                std::cout << "Chat server is not running" << std::endl;
            }
            break;
        }
        case 3: {
            if (chat_service.is_running()) {
                std::cout << "Chat server is running on port " << chat_service.get_port() << std::endl;
                std::cout << "Active clients: " << chat_service.get_client_count() << std::endl;
            } else {
                std::cout << "Chat server is not running" << std::endl;
            }
            break;
        }
        case 4:
            // Return to main menu
            break;
        default:
            std::cerr << "Invalid choice. Please try again." << std::endl;
    }
}
