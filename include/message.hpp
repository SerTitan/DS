#pragma once
#include <string>
#include <random>

inline std::string gen_uuid()
{
    static std::mt19937_64 rng{std::random_device{}()};
    static std::uniform_int_distribution<unsigned long long> dist;
    char buf[17];
    std::snprintf(buf, 17, "%016llx", dist(rng));
    return buf;
}

struct Message
{
    std::string type;
    std::string payload;
    std::string orig_id;
    long long   send_time;
    int         ttl;

    Message();
    Message(std::string t, std::string p, int ttl = 1);
    Message(std::string t, std::string p,
            long long ts, int ttl = 1);

    std::string to_json()      const;
    static Message from_json(const std::string&);
};
