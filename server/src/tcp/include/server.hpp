//
//  server.h
//  SysHealth
//
//  Created by Ramesh Kumar on 19/12/24.
//

#ifndef server_h
#define server_h

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <vector>
#include <mutex>
#include <signal.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <memory>
#include <iostream>

#define MAX_CONNECTIONS 10

class Server {
	
public:
	struct Client {
		SSL *ssl;
		int socket_fd;

	public:
		Client (SSL *s, int fd) : ssl(s), socket_fd(fd) {}

		~Client () {
			printf("=====> Client deleted : %d <=====\n", socket_fd);
			SSL_shutdown(ssl);
			SSL_free(ssl);
		}
	};

private:
	int server_fd;
	struct sockaddr_in address;
	
	SSL_CTX *ctx = NULL;
	SSL *ssl = NULL;
	
	std::mutex connection_mutex;
	std::vector<std::shared_ptr<Server::Client>> active_clients;
	
	static void process_interrupted(int reason);
	void accessfile();
	void accept_connections();

public:
	Server() { }
	~Server ()
	{
		active_clients.clear();
		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	}

	int start();
};

#endif /* server_h */
