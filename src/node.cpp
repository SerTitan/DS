#include "algorithm.hpp"
#include "server.hpp"
#include "stats.hpp"
#include "utils.hpp"
#include <thread>
#include <cstdlib>
#include <iostream>

using namespace std::chrono_literals;

int main() {
    std::string mode   = std::getenv("MODE") ? std::getenv("MODE") : "gossip";
    bool starter       = std::getenv("STARTER")
                       && std::string(std::getenv("STARTER")) == "1";
    int containers_cnt = std::getenv("containers_cnt")
                       ? std::stoi(std::getenv("containers_cnt")) : 100;

    if (mode == "watchdog") {
        run_watchdog();
        return 0;
    }

    Stats stats;
    auto  algo = AlgorithmFactory::create(mode, stats);

    Server server("239.20.0.1", 5001);
    server.subscribe("state_update",
        [&](const Message& m){ algo->on_receive(m); });

    if (starter) {
        for (int loss = 0; loss <= 90; loss += 10) {
            int ttl = (mode == "singlecast") ? containers_cnt + 10 : 10;
            for (int i = 0; i < 100; ++i) {
                Message m("state_update", std::to_string(loss), ttl);
                algo->broadcast(m);
            }
            std::this_thread::sleep_for(300ms);
        }
    }

    server.listen();
    return 0;
}
