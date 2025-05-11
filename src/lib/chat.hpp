#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>

// Chat client structure to store information about connected clients
struct ChatClient {
    int socket;
    std::string username;
    bool authenticated;
};

class ChatService {
private:
    int server_socket;
    unsigned int port;
    std::string auth_token;
    std::atomic<bool> running;
    std::thread server_thread;
    std::mutex clients_mutex;
    std::vector<ChatClient> clients;
    
    // Private methods
    void server_loop();
    void handle_client(int client_socket);
    bool authenticate_client(int client_socket);
    void broadcast_message(const std::string& message, int sender_socket = -1);
    void remove_client(int client_socket);

public:
    // Constructor and destructor
    ChatService(unsigned int port = 8989, const std::string& auth_token = "secure_token");
    ~ChatService();
    
    // Public methods
    bool start();
    void stop();
    bool is_running() const;
    unsigned int get_port() const;
    unsigned int get_client_count() const;
    
    // Static methods
    static void display_menu();
    static void handle_menu_choice();
};

// Function to integrate with the console manager
void add_chat_service_to_menu();

#endif // CHAT_SERVICE_H