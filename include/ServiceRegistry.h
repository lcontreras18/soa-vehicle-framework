#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

struct Message {
    std::string topic;
    std::string payload;
};

using SubscriberCallback = std::function<void(const Message&)>;

class ServiceRegistry {
public:
    static ServiceRegistry& getInstance() {
        static ServiceRegistry instance;
        return instance;
    }

    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;

    void registerService(const std::string& serviceName) {
        std::lock_guard<std::mutex> lock(mutex_);

        services_[serviceName] = true;

        std::cout
            << "[Registry] Service registered: "
            << serviceName << "\n";
    }

    void subscribe(
        const std::string& topic,
        const std::string& subscriberName,
        SubscriberCallback callback
    ) {
        std::lock_guard<std::mutex> lock(mutex_);

        subscribers_[topic].push_back({
            subscriberName,
            std::move(callback)
        });

        std::cout
            << "[Registry] '"
            << subscriberName
            << "' subscribed to topic: "
            << topic << "\n";
    }

    void publish(const Message& message) {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stopping_) {
                return;
            }

            messageQueue_.push(message);
        }

        condition_.notify_one();
    }

    void waitForIdle() {
        std::unique_lock<std::mutex> lock(mutex_);

        idleCondition_.wait(
            lock,
            [this]() {
                return messageQueue_.empty()
                    && activeCallbacks_ == 0;
            }
        );
    }

    void listServices() const {
        std::lock_guard<std::mutex> lock(mutex_);

        std::cout << "\n--- Registered Services ---\n";

        for (const auto& [name, _] : services_) {
            std::cout << "  * " << name << "\n";
        }

        std::cout << "---------------------------\n\n";
    }

    void reset() {
        waitForIdle();

        std::lock_guard<std::mutex> lock(mutex_);

        services_.clear();
        subscribers_.clear();

        while (!messageQueue_.empty()) {
            messageQueue_.pop();
        }
    }
    void setLoggingEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggingEnabled_ = enabled;
    }

private:
    ServiceRegistry() {
        dispatcherThread_ = std::thread(
            &ServiceRegistry::dispatchLoop,
            this
        );
    }

    ~ServiceRegistry() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }

        condition_.notify_all();

        if (dispatcherThread_.joinable()) {
            dispatcherThread_.join();
        }
    }

    void dispatchLoop() {
        while (true) {
            Message message;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                condition_.wait(
                    lock,
                    [this]() {
                        return stopping_
                            || !messageQueue_.empty();
                    }
                );

                if (stopping_ && messageQueue_.empty()) {
                    break;
                }

                message = messageQueue_.front();
                messageQueue_.pop();

                ++activeCallbacks_;
            }

            if (loggingEnabled_) {
                std::cout
                    << "[Bus] Dispatching topic='"
                    << message.topic
                    << "' payload='"
                    << message.payload << "'\n";
            }

            std::vector<
                std::pair<std::string, SubscriberCallback>
            > callbacks;

            {
                std::lock_guard<std::mutex> lock(mutex_);

                auto it = subscribers_.find(message.topic);

                if (it != subscribers_.end()) {
                    callbacks = it->second;
                }
            }

            for (auto& [subscriberName, callback] : callbacks) {
                if (loggingEnabled_) {
                std::cout
                    << "[Bus] -> "
                    << subscriberName << "\n";
            }

                callback(message);
            }

            {
                std::lock_guard<std::mutex> lock(mutex_);

                --activeCallbacks_;

                if (
                    messageQueue_.empty()
                    && activeCallbacks_ == 0
                ) {
                    idleCondition_.notify_all();
                }
            }
        }
    }

    std::unordered_map<std::string, bool> services_;

    std::unordered_map<
        std::string,
        std::vector<
            std::pair<std::string, SubscriberCallback>
        >
    > subscribers_;

    std::queue<Message> messageQueue_;

    mutable std::mutex mutex_;

    std::condition_variable condition_;
    std::condition_variable idleCondition_;

    std::thread dispatcherThread_;

    bool stopping_ = false;
    bool loggingEnabled_ = true;

    std::size_t activeCallbacks_ = 0;
};