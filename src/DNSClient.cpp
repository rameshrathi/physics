//
// Created by ramesh on 19/05/25.
//

#include "DNSClient.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

DNSClient::DNSClient(const std::string& dns_server_ip)
    : _dns_server_ip(dns_server_ip) {
    _sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sock_fd < 0)
        throw std::runtime_error("Failed to create socket");
}

std::vector<uint8_t> DNSClient::build_query(const std::string& hostname) {
    std::vector<uint8_t> query(512);
    uint16_t id = htons(0x1234);
    query[0] = id >> 8;
    query[1] = id & 0xFF;
    query[2] = 0x01; // recursion desired
    query[3] = 0x00;
    query[4] = 0x00;
    query[5] = 0x01; // QDCOUNT = 1
    query[6] = 0x00;
    query[7] = 0x00; // ANCOUNT
    query[8] = 0x00;
    query[9] = 0x00; // NSCOUNT
    query[10] = 0x00;
    query[11] = 0x00; // ARCOUNT

    size_t offset = 12;
    for (const auto& part : hostname.substr(0, hostname.size())) {
        auto dot = hostname.find('.', offset - 12);
        if (dot == std::string::npos) break;
        size_t len = dot - (offset - 12);
        query[offset++] = len;
        query.insert(query.begin() + offset, hostname.begin() + (offset - 12), hostname.begin() + dot);
        offset += len;
    }
    // final zero
    query[offset++] = 0;
    query[offset++] = 0x00;
    query[offset++] = 0x01; // QTYPE = A
    query[offset++] = 0x00;
    query[offset++] = 0x01; // QCLASS = IN

    query.resize(offset);
    return query;
}

void DNSClient::send_query(const std::vector<uint8_t>& query) {
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);
    inet_pton(AF_INET, _dns_server_ip.c_str(), &addr.sin_addr);

    sendto(_sock_fd, query.data(), query.size(), 0, (sockaddr*)&addr, sizeof(addr));
}

std::vector<uint8_t> DNSClient::receive_response() {
    std::vector<uint8_t> buffer(512);
    ssize_t len = recv(_sock_fd, buffer.data(), buffer.size(), 0);
    if (len < 0)
        throw std::runtime_error("Failed to receive response");
    buffer.resize(len);
    return buffer;
}

std::vector<DNSRecord> DNSClient::parse_response(const std::vector<uint8_t>& response) {
    std::vector<DNSRecord> records;
    size_t answer_count = (response[6] << 8) | response[7];
    size_t offset = 12;
    while (response[offset] != 0) offset += response[offset] + 1;
    offset += 5; // skip null + QTYPE(2) + QCLASS(2)

    for (size_t i = 0; i < answer_count; ++i) {
        offset += 2; // name pointer
        uint16_t type = (response[offset] << 8) | response[offset + 1];
        uint16_t cls = (response[offset + 2] << 8) | response[offset + 3];
        uint32_t ttl = ntohl(*(uint32_t*)&response[offset + 4]);
        uint16_t rdlength = (response[offset + 8] << 8) | response[offset + 9];
        offset += 10;

        std::string data;
        if (type == 1 && rdlength == 4) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &response[offset], ip, sizeof(ip));
            data = ip;
        }

        records.push_back({ "", type, cls, ttl, data });
        offset += rdlength;
    }
    return records;
}

std::vector<DNSRecord> DNSClient::resolve(const std::string& hostname) {
    auto query = build_query(hostname);
    send_query(query);
    auto response = receive_response();
    return parse_response(response);
}
