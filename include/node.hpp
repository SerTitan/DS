#pragma once
#include <string>

class Node {
public:
    explicit Node(const std::string& mode);
    void run();
private:
    std::string mode;
    bool is_watchdog;
};
