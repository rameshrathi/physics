#include "http_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 80

void send_http_request(const char* host, const char* request, size_t request_size) {
    int sock;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert host to binary address
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        close(sock);
        return;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return;
    }

    // Send HTTP request
    if (send(sock, request, request_size, 0) < 0) {
        perror("Send failed");
        close(sock);
        return;
    }

    // Read response
    char response[1024];
    ssize_t received;
    while ((received = recv(sock, response, sizeof(response) - 1, 0)) > 0) {
        response[received] = '\0';
        printf("%s", response);
    }

    if (received < 0) {
        perror("Receive failed");
    }

    close(sock);
}
