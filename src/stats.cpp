#include "stats.hpp"
#include "server.hpp"
#include "utils.hpp"
#include <filesystem>
#include <thread>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;
using namespace std::chrono_literals;

/* ───────── Stats ───────── */

Stats::Stats() {
    std::filesystem::create_directories("/logs");
    out.open("/logs/stats.csv", std::ios::trunc);
    out << "recv_time_ms,message_time_ms,latency_ms\n";
    std::cout << "[watchdog] stats.csv открыт\n";
}

Stats::~Stats() {
    out.close();

    json j;
    for (auto& [loss, v] : snaps_)
        for (auto& s : v)
            j[std::to_string(loss)].push_back({s.t, s.cnt});
    std::ofstream("/logs/sessions.json") << j.dump(2);

    for (auto& [loss, v] : snaps_) {
        std::ofstream f("/logs/stats_" + std::to_string(loss) + ".csv");
        for (auto& s : v) f << s.t << ',' << s.cnt << '\n';
    }
    std::cout << "[watchdog] sessions.json и stats_*.csv сохранены\n";
}

void Stats::log(const Message& msg)
{
    std::lock_guard lk(mtx);

    if (!seen_id_.insert(msg.orig_id).second) return;

    long long recv = now_ms();
    long long lat  = recv - msg.send_time;
    out << recv << ',' << msg.send_time << ',' << lat << '\n';

    int loss = std::stoi(msg.payload);
    if (!first_recv_.contains(loss)) first_recv_[loss] = recv;

    double t = (recv - first_recv_[loss]) / 1000.0;
    snaps_[loss].push_back({static_cast<long long>(snaps_[loss].size() + 1), t});

    updated_ = true;
}

/* ───────── Watchdog ───────── */

void run_watchdog()
{
    std::cout << "Watchdog collecting logs…\n";

    {
        Server server("239.20.0.1", 5001);
        Stats  stats;

        server.subscribe("messages_update",
            [&](const Message& m){ stats.log(m); });

        std::thread([&]{ server.listen(); }).detach();

        int idle = 0;
        while (idle < 10) {
            std::this_thread::sleep_for(1s);
            if (stats.updated_) { stats.updated_ = false; idle = 0; }
            else                 ++idle;
        }
        std::cout << "Watchdog done. CSV в /logs/stats.csv\n";
    }

    if (std::system("gnuplot /plot.gnuplot") == 0)
        std::cout << "plot.png построен\n";
    else
        std::cerr << "gnuplot: ошибка построения\n";
}
