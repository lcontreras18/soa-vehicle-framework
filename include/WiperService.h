#pragma once
#include "ServiceRegistry.h"
#include <iostream>
#include <string>

/**
 * WiperService
 * ------------
 * Controls wiper speed based on rain sensor input.
 * - Subscribes to "rain.sensor" topic
 * - Publishes "wiper.status"
 *
 * Wiper modes: OFF, LOW, HIGH
 */
class WiperService {
public:
    WiperService() {
        auto& registry = ServiceRegistry::getInstance();
        registry.registerService("WiperService");

        registry.subscribe("rain.sensor", "WiperService",
            [this](const Message& msg) {
                int intensity = std::stoi(msg.payload); // 0-100
                if (intensity == 0)       setMode("OFF");
                else if (intensity < 50)  setMode("LOW");
                else                      setMode("HIGH");
            });
    }

private:
    std::string mode_ = "OFF";

    void setMode(const std::string& mode) {
        if (mode_ == mode) return; // no change
        mode_ = mode;
        std::cout << "[WiperService] Wiper mode set to: " << mode_ << "\n";
        ServiceRegistry::getInstance().publish({"wiper.status", mode_});
    }
};
