//
//  server.cpp
//  SysHealth
//


#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <signal.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <memory>

#include "include/server.hpp"

void  init_ssl() {
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
}

SSL_CTX* create_context() {
	const SSL_METHOD *method = TLS_server_method();
	SSL_CTX *ctx = SSL_CTX_new(method);
	if (!ctx) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("Unable to create SSL context");
	}
	return ctx;
}

void configure_context(const char * cert_file, const char * key_file, SSL_CTX * ssl_ctx) {
	if (SSL_CTX_use_certificate_file(ssl_ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("Failed to load certificate");
	}
	if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("Failed to load private key");
	}
}

void Server :: accept_connections() {
	int client_socket_fd;
	int addrlen = sizeof(address);

	// Accept up to MAX_CONNECTIONS
	while (true) {
		if ((client_socket_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			perror("Accept failed");
			continue;
		}
		// SSL connection with newly connected client
		SSL *ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client_socket_fd);
		
		// Run in deteched for bg communication
		Server::Client * client = new Server::Client(ssl, client_socket_fd);
		std::shared_ptr<Server::Client> ptr_client(client);
		
		std::cout << "New connection accepted: " << client_socket_fd << std::endl;
		{
			std::lock_guard<std::mutex> lock(connection_mutex);
			active_clients.push_back(ptr_client);
		}
		
		// Run message handler in detected thread
		auto msg_handler = [] (std::shared_ptr<Server::Client> ptr) {
			const int buffer_size = 1024;
			char buffer[buffer_size];
			
			if (SSL_accept(ptr->ssl) <= 0) {
				ERR_print_errors_fp(stderr);
			} else {
				while (true) {
					memset(buffer, 0, buffer_size);
					int bytes = SSL_read(ptr->ssl, buffer, buffer_size - 1);
					if (bytes <= 0) {
						int err = SSL_get_error(ptr->ssl, bytes);
						if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
							continue;
						}
						std::cerr << "SSL read error: " << err << std::endl;
						break;
					}
					std::cout << "Received: " << buffer << std::endl;
					// Send back same message
					SSL_write(ptr->ssl, buffer, bytes);
				}
			}
		};
		std::thread(msg_handler, ptr_client).detach();
		
		// Remove old connection if MAX_CONNECTIONS reached
		if (active_clients.size() > MAX_CONNECTIONS) {
			std::cout << "Max connections reached => Removing oldest connection" << std::endl;
			std::lock_guard<std::mutex> lock(connection_mutex);
			active_clients.erase(active_clients.begin());
		}
	}
}

int Server :: start() {
	
	// config
	const int port = 4433;
	const char * cert_file = "/Users/ramesh/.local/ssl/server.crt";
	const char * key_file = "/Users/ramesh/.local/ssl/server.key";
	
	// SIGNALS
	// Ignore SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	// Detect interrupt
	signal(SIGINT, process_interrupted);
	
	// SSL Context
	init_ssl();
	ctx = create_context();
	configure_context(cert_file, key_file, ctx);
	
	// Create socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Socket failed");
		return EXIT_FAILURE;
	}
	
	// Set socket options
	int opt = 1;
	// Set SO_REUSEADDR
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt SO_REUSEADDR");
		return EXIT_FAILURE;
	}
	
	// Set SO_REUSEPORT (check if it is supported on macOS)
#ifdef SO_REUSEPORT
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		perror("setsockopt SO_REUSEPORT");
		return EXIT_FAILURE;
	}
#endif
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	// Bind the socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Bind failed");
		return EXIT_FAILURE;
	}
	
	// Start listening for connections
	if (listen(server_fd, 10) < 0) {
		perror("Listen failed");
		return EXIT_FAILURE;
	}
	
	std::cout << "Server listening on port : " << port << std::endl;
	
	// Start accepting connections
	accept_connections();
	
	return 0;
}

void Server :: process_interrupted(int reason) {
	pid_t pid = getpid();
	std::cout << "Process : " << pid << " , interrupted : " << reason << std::endl;
}

void Server :: accessfile() {
	std::cout << "DEBUG: accessfile() called by process " << ::getpid() << " (parent: " << ::getppid() << ")" << std::endl;
	// Code here to create and/or access the file
}
