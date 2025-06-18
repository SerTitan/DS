#include "algorithm.hpp"
#include <iostream>
#include <random>
#include <algorithm>

class BaseAlgorithm : public IAlgorithm {
protected:
    Sender sender;
    Stats& stats;
    std::vector<std::string> peers;

    void discover_peers() {
        for (int i = 2; i <= 100; ++i)
            peers.push_back("172.31.0." + std::to_string(i));
    }

    void notify_watchdog(const Message& msg) {
        Message rep("messages_update", msg.payload, 2);
        rep.orig_id   = msg.orig_id;
        rep.send_time = msg.send_time;
        sender.send("239.20.0.1", 5001, rep.to_json());
    }

public:
    explicit BaseAlgorithm(Stats& s) : stats(s) { discover_peers(); }
};

/* ───────── Singlecast ───────── */

class Singlecast : public BaseAlgorithm {
public:
    using BaseAlgorithm::BaseAlgorithm;

    void on_receive(const Message& msg) override {
        stats.log(msg);
        broadcast(msg);
    }

    void broadcast(const Message& msg) override {
        if (peers.empty()) return;
        sender.send(peers.front(), 5001, msg.to_json());
        notify_watchdog(msg);
    }
};

/* ───────── Multicast ───────── */

class Multicast : public BaseAlgorithm {
public:
    using BaseAlgorithm::BaseAlgorithm;
    void on_receive(const Message& msg) override { stats.log(msg); broadcast(msg); }
    void broadcast (const Message& msg) override {
        sender.send("239.10.0.1", 5001, msg.to_json());
        notify_watchdog(msg);
    }
};

/* ───────── Broadcast ───────── */

class Broadcast : public BaseAlgorithm {
public:
    using BaseAlgorithm::BaseAlgorithm;
    void on_receive(const Message& msg) override { stats.log(msg); broadcast(msg); }
    void broadcast (const Message& msg) override {
        sender.send("224.1.1.1", 5001, msg.to_json());
        notify_watchdog(msg);
    }
};

/* ───────── Gossip (3 случайных соседа) ───────── */

class Gossip : public BaseAlgorithm {
public:
    using BaseAlgorithm::BaseAlgorithm;
    void on_receive(const Message& msg) override { stats.log(msg); broadcast(msg); }

    void broadcast(const Message& msg) override {
        std::shuffle(peers.begin(), peers.end(), std::mt19937{std::random_device{}()});
        for (int i = 0; i < 3 && i < (int)peers.size(); ++i)
            sender.send(peers[i], 5001, msg.to_json());
        notify_watchdog(msg);
    }
};

/* ───────── фабрика ───────── */

std::shared_ptr<IAlgorithm> AlgorithmFactory::create(const std::string& mode,
                                                     Stats& stats)
{
    if (mode == "singlecast") return std::make_shared<Singlecast>(stats);
    if (mode == "multicast")  return std::make_shared<Multicast>(stats);
    if (mode == "broadcast")  return std::make_shared<Broadcast>(stats);
    return std::make_shared<Gossip>(stats);
}
