#pragma once
#include "message.hpp"
#include <fstream>
#include <mutex>
#include <map>
#include <vector>
#include <unordered_set>

struct Snap { long long cnt; double t; };

class Stats {
public:
    Stats();
    ~Stats();

    void log(const Message& msg);
    bool updated_{false};

private:
    std::ofstream out;
    std::mutex    mtx;

    std::unordered_set<std::string> seen_id_;
    std::map<int,long long>         first_recv_;
    std::map<int,std::vector<Snap>> snaps_;
};

/* запуск watchdog */
void run_watchdog();
