#pragma once
#include "ServiceRegistry.h"
#include <iostream>

/**
 * InteriorLightingService
 * -----------------------
 * Controls cabin lighting based on door lock state and door events.
 */
class InteriorLightingService {
public:
    InteriorLightingService() {
        auto& registry = ServiceRegistry::getInstance();
        registry.registerService("InteriorLightingService");

        // React to door lock changes
        registry.subscribe("door.lock.status", "InteriorLightingService",
            [this](const Message& msg) {
                if (msg.payload == "UNLOCKED") {
                    setLight(true, "Door unlocked");
                } else {
                    setLight(false, "Door locked");
                }
            });

        // React to door open/close events
        registry.subscribe("door.open.event", "InteriorLightingService",
            [this](const Message& msg) {
                if (msg.payload == "OPEN") {
                    setLight(true, "Door opened");
                } else if (msg.payload == "CLOSED" && !doorUnlocked_) {
                    setLight(false, "Door closed and locked");
                }
            });
    }

private:
    bool lightOn_ = false;
    bool doorUnlocked_ = false;

    void setLight(bool on, const std::string& reason) {
        lightOn_ = on;
        std::string status = on ? "ON" : "OFF";
        std::cout << "[InteriorLighting] Light " << status << " (" << reason << ")\n";
        ServiceRegistry::getInstance().publish({"interior.light.status", status});
    }
};
