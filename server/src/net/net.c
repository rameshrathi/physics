#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>

// Function to calculate ICMP checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    int sockfd;
    struct sockaddr_in dest_addr;
    char packet[sizeof(struct iphdr) + sizeof(struct icmphdr) + 64]; // IP header + ICMP header + 64 bytes data
    struct iphdr *ip = (struct iphdr *)packet;
    struct icmphdr *icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));
    char *data = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Zero out the packet buffer
    memset(packet, 0, sizeof(packet));

    // Fill IP header (minimal, kernel may override some fields)
    ip->ihl = 5;           // Header length (5 words = 20 bytes)
    ip->version = 4;       // IPv4
    ip->tos = 0;           // Type of service
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr) + 64); // Total length
    ip->id = htons(12345); // Identification
    ip->frag_off = 0;      // Fragment offset
    ip->ttl = 64;          // Time to live
    ip->protocol = IPPROTO_ICMP; // Protocol
    ip->saddr = inet_addr("192.168.1.100"); // Source IP (replace with your IP)
    ip->daddr = inet_addr("8.8.8.8");      // Destination IP (e.g., Google DNS)

    // Fill ICMP header (Echo Request)
    icmp->type = ICMP_ECHO;      // Type 8
    icmp->code = 0;              // Code 0
    icmp->un.echo.id = htons(1); // Identifier
    icmp->un.echo.sequence = htons(1); // Sequence number
    icmp->checksum = 0;          // Set to 0 before checksum calculation

    // Fill data (optional payload)
    strncpy(data, "Hello, this is an ICMP Echo Request!", 64);

    // Calculate ICMP checksum (header + data)
    icmp->checksum = checksum(icmp, sizeof(struct icmphdr) + 64);

    // Set destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = ip->daddr;

    // Send the packet
    int bytes_sent = sendto(sockfd, packet, ntohs(ip->tot_len), 0,
                            (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (bytes_sent < 0) {
        perror("Sendto failed");
        close(sockfd);
        exit(1);
    }

    printf("Sent %d bytes of ICMP Echo Request\n", bytes_sent);
    close(sockfd);
    return 0;
}