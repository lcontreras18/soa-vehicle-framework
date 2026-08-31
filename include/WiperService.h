#pragma once

#include "ServiceRegistry.h"

#include <iostream>
#include <string>

/**
 * WiperService
 * ------------
 * Controls wiper speed based on rain sensor input.
 * Valid rain intensity range: 0-100
 * Wiper modes: OFF, LOW, HIGH
 */
class WiperService {

public:
    WiperService() {
        auto& registry = ServiceRegistry::getInstance();

        registry.registerService("WiperService");

        registry.subscribe(
            "rain.sensor",
            "WiperService",
            [this](const Message& msg) {

                try {
                    int intensity = std::stoi(msg.payload);

                    if (intensity < 0 || intensity > 100) {
                        std::cout
                            << "[WiperService] Invalid rain intensity: "
                            << intensity
                            << " (expected 0-100)\n";
                        return;
                    }

                    if (intensity == 0) {
                        setMode("OFF");
                    }
                    else if (intensity < 50) {
                        setMode("LOW");
                    }
                    else {
                        setMode("HIGH");
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout
                        << "[WiperService] Invalid rain sensor payload: "
                        << msg.payload << "\n";
                }
                catch (const std::out_of_range&) {
                    std::cout
                        << "[WiperService] Rain sensor payload out of range: "
                        << msg.payload << "\n";
                }
            }
        );
    }
    std::string getMode() const {
        return mode_;
    }

private:
    std::string mode_ = "OFF";

    void setMode(const std::string& mode) {

        if (mode_ == mode) {
            return;
        }

        mode_ = mode;

        std::cout
            << "[WiperService] Wiper mode set to: "
            << mode_ << "\n";

        ServiceRegistry::getInstance().publish({
            "wiper.status",
            mode_
        });
    }
};
