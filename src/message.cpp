#include "message.hpp"
#include "utils.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


Message::Message() = default;

Message::Message(std::string t, std::string p, int ttl_)
    : type(std::move(t)),
      payload(std::move(p)),
      orig_id(gen_uuid()),
      send_time(now_ms()),
      ttl(ttl_) {}


Message::Message(std::string t, std::string p,
                 long long ts, int ttl_)
    : type(std::move(t)),
      payload(std::move(p)),
      orig_id(gen_uuid()),
      send_time(ts),
      ttl(ttl_) {}

/* ---------- сериализация ---------- */

std::string Message::to_json() const
{
    json j{{"type",       type},
           {"payload",    payload},
           {"orig_id",    orig_id},
           {"send_time",  send_time},
           {"ttl",        ttl}};
    return j.dump();
}

Message Message::from_json(const std::string& s)
{
    json j = json::parse(s);
    Message m;
    m.type      = j.value("type", "");
    m.payload   = j.value("payload", "");
    m.orig_id   = j.value("orig_id", "");
    m.send_time = j.value("send_time", 0LL);
    m.ttl       = j.value("ttl", 1);
    return m;
}
