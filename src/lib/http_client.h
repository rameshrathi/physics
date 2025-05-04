#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <stddef.h>

// Send HTTP request using a socket
void send_http_request(const char* host, const char* request, size_t request_size);

#endif
