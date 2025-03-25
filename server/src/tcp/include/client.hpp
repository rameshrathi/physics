//
//  client.hpp
//
//  Created by Ramesh Kumar on 31/12/24.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <set>

#include "server.hpp"

class Client {

	const char * host;
	const int port;

	int socket_fd = 0;
	SSL * ssl = NULL;
	SSL_CTX *ctx = NULL;
	
public:
	// Constructor
	Client ( const char * h, const int p ) : host(h), port(p) {}

	// Desctructor
	~Client () {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		EVP_cleanup();
	}

public:
	int connect();
	int send_message(const char * msg);
	int read_message_with_wait(char * msg, int len);
};

#endif
