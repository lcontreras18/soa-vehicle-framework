#include "ServiceRegistry.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

int main() {
    auto& bus = ServiceRegistry::getInstance();

    bus.setLoggingEnabled(false);

    constexpr std::size_t MESSAGE_COUNT = 10000;

    using Clock = std::chrono::steady_clock;

    std::vector<Clock::time_point> sendTimes(MESSAGE_COUNT);
    std::vector<double> latencies;

    latencies.reserve(MESSAGE_COUNT);

    bus.subscribe(
        "benchmark.latency",
        "BenchmarkSubscriber",
        [&](const Message& msg) {
            std::size_t id =
                static_cast<std::size_t>(
                    std::stoul(msg.payload)
                );

            auto receivedAt = Clock::now();

            double latency =
                std::chrono::duration<double, std::micro>(
                    receivedAt - sendTimes[id]
                ).count();

            latencies.push_back(latency);
        }
    );

    auto benchmarkStart = Clock::now();

    for (std::size_t i = 0; i < MESSAGE_COUNT; ++i) {
        sendTimes[i] = Clock::now();

        bus.publish({
            "benchmark.latency",
            std::to_string(i)
        });
    }

    bus.waitForIdle();

    auto benchmarkEnd = Clock::now();

    std::sort(latencies.begin(), latencies.end());

    double totalLatency = 0.0;

    for (double latency : latencies) {
        totalLatency += latency;
    }

    double average =
        totalLatency / latencies.size();

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

    double seconds =
        std::chrono::duration<double>(
            benchmarkEnd - benchmarkStart
        ).count();

    double throughput =
        MESSAGE_COUNT / seconds;

    std::cout << "\n=== Message Bus Latency Benchmark ===\n";

    std::cout
        << "Messages:    "
        << MESSAGE_COUNT << "\n";

    std::cout
        << "Throughput:  "
        << throughput
        << " messages/sec\n";

    std::cout
        << "Average:     "
        << average
        << " us\n";

    std::cout
        << "Median:      "
        << median
        << " us\n";

    std::cout
        << "P95:         "
        << p95
        << " us\n";

    std::cout
        << "P99:         "
        << p99
        << " us\n";

    std::cout
        << "Maximum:     "
        << maximum
        << " us\n";

    bus.reset();

    return 0;
}