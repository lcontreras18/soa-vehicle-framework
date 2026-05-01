#include "ServiceRegistry.h"
#include "DoorLockService.h"
#include "InteriorLightingService.h"
#include "WiperService.h"
#include <iostream>
#include <thread>
#include <chrono>

void separator(const std::string& label) {
    std::cout << "\n========== " << label << " ==========\n";
}

int main() {
    std::cout << "=== Karma SDV Body Control SoA Framework ===\n\n";

    // --- Boot: instantiate all services ---
    // Each service self-registers and subscribes to its topics
    DoorLockService      doorLock;
    InteriorLightingService interiorLight;
    WiperService         wipers;

    auto& bus = ServiceRegistry::getInstance();
    bus.listServices();

    // --- Scenario 1: Key fob unlock ---
    separator("Scenario 1: Key Fob UNLOCK");
    bus.publish({"key.fob.command", "UNLOCK"});

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // --- Scenario 2: Door opens ---
    separator("Scenario 2: Door Opens");
    bus.publish({"door.open.event", "OPEN"});

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // --- Scenario 3: Door closes, key fob locks ---
    separator("Scenario 3: Door Closes + Key Fob LOCK");
    bus.publish({"door.open.event", "CLOSED"});
    bus.publish({"key.fob.command", "LOCK"});

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // --- Scenario 4: Vehicle speed triggers auto-lock ---
    separator("Scenario 4: Vehicle Reaches 25 mph (Auto-Lock)");
    bus.publish({"key.fob.command", "UNLOCK"}); // unlock first
    bus.publish({"vehicle.speed", "25.0"});

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // --- Scenario 5: Rain sensor activates wipers ---
    separator("Scenario 5: Rain Sensor Events");
    bus.publish({"rain.sensor", "0"});   // no rain
    bus.publish({"rain.sensor", "30"});  // light rain
    bus.publish({"rain.sensor", "80"});  // heavy rain
    bus.publish({"rain.sensor", "0"});   // rain stops

    std::cout << "\n=== Simulation Complete ===\n";
    return 0;
}
