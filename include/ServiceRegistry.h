#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <any>
#include <mutex>
#include <iostream>

// A "Message" is just a topic + payload string
struct Message {
    std::string topic;
    std::string payload;
};

// Callback type for subscribers
using SubscriberCallback = std::function<void(const Message&)>;

class ServiceRegistry {
public:
    // Singleton: one registry for the whole system
    static ServiceRegistry& getInstance() {
        static ServiceRegistry instance;
        return instance;
    }

    // Register a named service
    void registerService(const std::string& serviceName) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        services_[serviceName] = true;
        std::cout << "[Registry] Service registered: " << serviceName << "\n";
    }

    // Subscribe to a topic 
    void subscribe(const std::string& topic, const std::string& subscriberName, SubscriberCallback cb) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        subscribers_[topic].push_back({subscriberName, cb});
        std::cout << "[Registry] '" << subscriberName << "' subscribed to topic: " << topic << "\n";
    }

    // Publish a message to all subscribers of a specific topic
    void publish(const Message& msg) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        std::cout << "[Bus] Publishing topic='" << msg.topic << "' payload='" << msg.payload << "'\n";
        auto it = subscribers_.find(msg.topic);
        if (it != subscribers_.end()) {
            for (auto& [name, cb] : it->second) {
                cb(msg);
            }
        }
    }

    // List all registered services
    void listServices() const {
        std::cout << "\n--- Registered Services ---\n";
        for (auto& [name, _] : services_) {
            std::cout << "  * " << name << "\n";
        }
        std::cout << "---------------------------\n\n";
    }

private:
    ServiceRegistry() = default;
    std::unordered_map<std::string, bool> services_;
    std::unordered_map<std::string, std::vector<std::pair<std::string, SubscriberCallback>>> subscribers_;
    mutable std::recursive_mutex mutex_;
};
