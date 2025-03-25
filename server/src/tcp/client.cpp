//
//  client.cpp
//  SysHealth
//

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <cstring>

#include "include/client.hpp"

void wait_for_message(SSL *ssl) {
	while (1) {
		char buffer[1024] = {0};
		int bytes = SSL_read(ssl, buffer, sizeof(buffer));
		if (bytes > 0) {
			std::cout << "Received: " << buffer << std::endl;
		} else {
			exit(EXIT_FAILURE);
		}
	}
}

int set_socket_timeout(int sockfd, int timeout_seconds) {
	struct timeval timeout;
	timeout.tv_sec = timeout_seconds;
	timeout.tv_usec = 0;
	
	// Set the timeout for read operations
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("setsockopt");
		return -1;
	}
	
	// Set the timeout for write operations (optional)
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
		perror("setsockopt");
		return -1;
	}
	
	return 0;
}

SSL_CTX *create_context(const char *ca_cert_path) {
	// Create a new SSL_CTX object
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	
	// Load the trusted certificate
	if (SSL_CTX_load_verify_locations(ctx, ca_cert_path, NULL) != 1) {
		fprintf(stderr, "Error loading CA certificate file\n");
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ctx);
		exit(EXIT_FAILURE);
	}
	
	return ctx;
}

int Client :: connect() {
	
	// Initialize OpenSSL
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
	
	// Create TCP socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Socket creation failed");
		return EXIT_FAILURE;
	}
	
	struct sockaddr_in server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	
	// set timeout
//	set_socket_timeout(socket_fd, 30);
	
	// Convert server address to binary form
	if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
		perror("Invalid address or address not supported");
		return EXIT_FAILURE;
	}

	// Connect to the server
	if (::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("Connection failed");
		return EXIT_FAILURE;
	}
	
	// Create SSL context
	const char * cert_file = "/Users/ramesh/.local/ssl/server.crt";
	ctx = create_context(cert_file);
	if (!ctx) {
		perror("SSL Context creation failed");
		ERR_print_errors_fp(stderr);
		return EXIT_FAILURE;
	}

	// Set up SSL on the socket
	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, socket_fd);
	SSL_set_verify(ssl, SSL_VERIFY_NONE, NULL);

	if (SSL_connect(ssl) <= 0) {
		perror("SSL handshake failed");
		ERR_print_errors_fp(stderr);
		close(socket_fd);
		return EXIT_FAILURE;
	}
	// Verify the server's certificate
	if (SSL_get_verify_result(ssl) != X509_V_OK) {
		fprintf(stderr, "Certificate verification failed\n");
		SSL_free(ssl);
		close(socket_fd);
		return EXIT_FAILURE;
	}
	std::cout << "Connected to server with SSL" << std::endl;

	// setup wait for message on another thread
	std::thread th(wait_for_message, ssl);
	th.detach();

	return 0;
}

//  ===== Send secure message =====
int Client :: send_message ( const char *msg ) {
	// Secure communication
	int status = SSL_write(ssl, msg, (int)strlen(msg));
	if (status != 0) {
		perror("Message send failed");
		return EXIT_FAILURE;
	}
	return 0;
}

int Client :: read_message_with_wait(char * msg, int len) {
	// Wait for message from server
	int status = SSL_read(ssl, msg, len);
	if (status != 0) {
		perror("Message send failed");
		return EXIT_FAILURE;
	}
	return 0;
}
