#include <iostream>

#include "DNSClient.h"

int main() {
    DNSClient client("8.8.8.8"); // Google's DNS
    auto records = client.resolve("www.google.com");
    for (const auto& record : records) {
        if (record.type == 1) // A record
            std::cout << "IP: " << record.data << "\n";
    }
    return 0;
}
