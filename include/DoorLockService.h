#pragma once
#include "ServiceRegistry.h"
#include <iostream>
#include <string>

/**
 * DoorLockService
 * ---------------
 * Responsible for managing door lock states.
 * - Subscribes to "key.fob.command" to react to remote key events
 * - Publishes "door.lock.status" when lock state changes
 *
 * In a real BCM, this would interface with LIN/CAN actuators.
 */
class DoorLockService {
public:
    DoorLockService() {
        auto& registry = ServiceRegistry::getInstance();
        registry.registerService("DoorLockService");

        // Subscribe to key fob commands
        registry.subscribe("key.fob.command", "DoorLockService",
            [this](const Message& msg) {
                handleKeyFobCommand(msg.payload);
            });

        // Subscribe to vehicle speed — auto-lock above 10 mph
        registry.subscribe("vehicle.speed", "DoorLockService",
            [this](const Message& msg) {
                float speed = std::stof(msg.payload);
                if (speed > 10.0f && !locked_) {
                    std::cout << "[DoorLockService] Speed > 10 mph — auto-locking doors\n";
                    setLocked(true);
                }
            });
    }

    void setLocked(bool lock) {
        locked_ = lock;
        std::string status = locked_ ? "LOCKED" : "UNLOCKED";
        std::cout << "[DoorLockService] Doors are now: " << status << "\n";

        // Publish the new status so other services can react
        ServiceRegistry::getInstance().publish({"door.lock.status", status});
    }

private:
    bool locked_ = false;

    void handleKeyFobCommand(const std::string& cmd) {
        if (cmd == "LOCK")   setLocked(true);
        else if (cmd == "UNLOCK") setLocked(false);
        else std::cout << "[DoorLockService] Unknown command: " << cmd << "\n";
    }
};
