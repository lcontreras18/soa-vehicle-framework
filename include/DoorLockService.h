#pragma once

#include "ServiceRegistry.h"

#include <iostream>
#include <string>

/**
 * DoorLockService
 * ---------------
 * Responsible for managing door lock states.
 */
class DoorLockService {

public:
    DoorLockService() {
        auto& registry = ServiceRegistry::getInstance();

        registry.registerService("DoorLockService");

        
        registry.subscribe(
            "key.fob.command",
            "DoorLockService",
            [this](const Message& msg) {
                handleKeyFobCommand(msg.payload);
            }
        );

        registry.subscribe(
            "vehicle.speed",
            "DoorLockService",
            [this](const Message& msg) {

                try {
                    float speed = std::stof(msg.payload);

                    
                    if (speed < 0.0f) {
                        std::cout
                            << "[DoorLockService] Invalid vehicle speed: "
                            << msg.payload << "\n";
                        return;
                    }

                    if (speed > 10.0f && !locked_) {
                        std::cout
                            << "[DoorLockService] Speed > 10 mph - auto-locking doors\n";

                        setLocked(true);
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout
                        << "[DoorLockService] Invalid speed payload: "
                        << msg.payload << "\n";
                }
                catch (const std::out_of_range&) {
                    std::cout
                        << "[DoorLockService] Speed payload out of range: "
                        << msg.payload << "\n";
                }
            }
        );
    }

private:
    bool locked_ = false;

    void setLocked(bool lock) {

        
        if (locked_ == lock) {
            return;
        }

        locked_ = lock;

        std::string status = locked_ ? "LOCKED" : "UNLOCKED";

        std::cout
            << "[DoorLockService] Doors are now: "
            << status << "\n";

        ServiceRegistry::getInstance().publish({
            "door.lock.status",
            status
        });
    }

    void handleKeyFobCommand(const std::string& cmd) {

        if (cmd == "LOCK") {
            setLocked(true);
        }
        else if (cmd == "UNLOCK") {
            setLocked(false);
        }
        else {
            std::cout
                << "[DoorLockService] Unknown key fob command: "
                << cmd << "\n";
        }
    }
};
