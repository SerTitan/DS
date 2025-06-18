#include "server.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <random>

Server::Server(const std::string& group, int port)
    : group(group), port(port) {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    ip_mreq mreq{};
    inet_pton(AF_INET, group.c_str(), &mreq.imr_multiaddr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}

void Server::subscribe(const std::string& type, Callback cb) {
    std::cout << "[server] registered subscription for: " << type << std::endl;
    subscriptions[type].push_back(std::move(cb));
}

void Server::listen() {
    std::cout << "Listening on " << group << ":" << port << std::endl;

    char buffer[2048];
    while (true) {
        ssize_t len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len < 0) continue;

        buffer[len] = '\0';
        std::string raw(buffer);

        std::cout << "[server] received raw: " << raw << std::endl;

        try {
            Message msg = Message::from_json(raw);
            std::cout << "[server] decoded msg: " << msg.to_json() << std::endl;
            std::cout << "[server] msg.type = '" << msg.type << "'" << std::endl;

            msg.ttl -= 1;
            if (msg.ttl < 0) {
                std::cout << "[server] msg dropped due to TTL" << std::endl;
                continue;
            }

            auto it = subscriptions.find(msg.type);
            if (it != subscriptions.end()) {
                std::cout << "[server] found subscription for type: " << msg.type << std::endl;
                for (auto& cb : it->second) {
                    cb(msg);
                }
            } else {
                std::cout << "[server] no subscribers for type: " << msg.type << std::endl;
            }

        } catch (...) {
            std::cerr << "[server] failed to parse message.\n";
        }
    }
}
