#pragma once
#include "message.hpp"
#include <functional>
#include <string>
#include <map>
#include <vector>

class Server {
public:
    using Callback = std::function<void(const Message&)>;

    Server(const std::string& multicast_group, int port = 5001);
    void subscribe(const std::string& type, Callback cb);
    void listen();

private:
    int sock;
    std::string group;
    int port;
    std::map<std::string, std::vector<Callback>> subscriptions;
};
