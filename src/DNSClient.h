//
// Created by ramesh on 19/05/25.
//

#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct DNSRecord {
    std::string name;
    uint16_t type;
    uint16_t cls;
    uint32_t ttl;
    std::string data;
};

class DNSClient {
public:
    DNSClient(const std::string& dns_server_ip);
    std::vector<DNSRecord> resolve(const std::string& hostname);

private:
    std::vector<uint8_t> build_query(const std::string& hostname);
    void send_query(const std::vector<uint8_t>& query);
    std::vector<uint8_t> receive_response();
    std::vector<DNSRecord> parse_response(const std::vector<uint8_t>& response);

    std::string _dns_server_ip;
    int _sock_fd;
};

