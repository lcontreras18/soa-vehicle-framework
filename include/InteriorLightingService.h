#pragma once

#include "ServiceRegistry.h"

#include <iostream>
#include <string>

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
        registry.subscribe(
            "door.lock.status",
            "InteriorLightingService",
            [this](const Message& msg) {

                if (msg.payload == "UNLOCKED") {
                    doorUnlocked_ = true;
                    setLight(true, "Door unlocked");
                }
                else if (msg.payload == "LOCKED") {
                    doorUnlocked_ = false;
                    setLight(false, "Door locked");
                }
                else {
                    std::cout
                        << "[InteriorLighting] Unknown door lock status: "
                        << msg.payload << "\n";
                }
            }
        );

        // React to door open/close events
        registry.subscribe(
            "door.open.event",
            "InteriorLightingService",
            [this](const Message& msg) {

                if (msg.payload == "OPEN") {
                    setLight(true, "Door opened");
                }
                else if (msg.payload == "CLOSED") {

                    // Only turn the light off when the vehicle is locked
                    if (!doorUnlocked_) {
                        setLight(false, "Door closed and locked");
                    }
                }
                else {
                    std::cout
                        << "[InteriorLighting] Unknown door event: "
                        << msg.payload << "\n";
                }
            }
        );
    }

private:
    bool lightOn_ = false;
    bool doorUnlocked_ = false;

    void setLight(bool on, const std::string& reason) {

        lightOn_ = on;

        std::string status = lightOn_ ? "ON" : "OFF";

        std::cout
            << "[InteriorLighting] Light "
            << status
            << " ("
            << reason
            << ")\n";

        // Publish lighting state so other services can react
        ServiceRegistry::getInstance().publish({
            "interior.light.status",
            status
        });
    }
};
