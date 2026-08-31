#include "ServiceRegistry.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

int main() {
    auto& bus = ServiceRegistry::getInstance();

    bus.setLoggingEnabled(false);

    constexpr std::size_t MESSAGE_COUNT = 10000;

    using Clock = std::chrono::steady_clock;

    std::vector<double> latencies;
    latencies.reserve(MESSAGE_COUNT);

    Clock::time_point sentAt;
    Clock::time_point receivedAt;

    bus.subscribe(
        "benchmark.single",
        "LatencySubscriber",
        [&](const Message&) {
            receivedAt = Clock::now();
        }
    );

    for (std::size_t i = 0; i < MESSAGE_COUNT; ++i) {
        sentAt = Clock::now();

        bus.publish({
            "benchmark.single",
            "ping"
        });

        bus.waitForIdle();

        double latency =
            std::chrono::duration<double, std::micro>(
                receivedAt - sentAt
            ).count();

        latencies.push_back(latency);
    }

    std::sort(latencies.begin(), latencies.end());

    double total = 0.0;

    for (double latency : latencies) {
        total += latency;
    }

    double average =
        total / latencies.size();

    double median =
        latencies[latencies.size() / 2];

    double p95 =
        latencies[
            static_cast<std::size_t>(
                (latencies.size() - 1) * 0.95
            )
        ];

    double p99 =
        latencies[
            static_cast<std::size_t>(
                (latencies.size() - 1) * 0.99
            )
        ];

    double maximum =
        latencies.back();

    std::cout << "\n=== Unloaded Message Latency Benchmark ===\n";

    std::cout
        << "Messages: "
        << MESSAGE_COUNT << "\n";

    std::cout
        << "Average:  "
        << average << " us\n";

    std::cout
        << "Median:   "
        << median << " us\n";

    std::cout
        << "P95:      "
        << p95 << " us\n";

    std::cout
        << "P99:      "
        << p99 << " us\n";

    std::cout
        << "Maximum:  "
        << maximum << " us\n";

    bus.reset();

    return 0;
}