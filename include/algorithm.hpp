#pragma once
#include "message.hpp"
#include "sender.hpp"
#include "stats.hpp"
#include <memory>
#include <vector>
#include <string>

class IAlgorithm {
public:
    virtual ~IAlgorithm() = default;
    virtual void on_receive(const Message& msg) = 0;
    virtual void broadcast(const Message& msg) = 0;
};

class AlgorithmFactory {
public:
    static std::shared_ptr<IAlgorithm> create(const std::string& mode, Stats& stats);
};
