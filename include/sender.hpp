#pragma once
#include <string>

class Sender {
public:
    Sender();
    void send(const std::string& address, int port, const std::string& message);
private:
    int sock;
};
