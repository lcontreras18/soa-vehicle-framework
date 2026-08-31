#pragma once

#include "CanFrame.h"
#include "ServiceRegistry.h"

#include <cstdint>
#include <string>

class CanGateway {
public:
    static constexpr std::uint32_t KEY_FOB_COMMAND_ID = 0x100;
    static constexpr std::uint32_t VEHICLE_SPEED_ID = 0x101;
    static constexpr std::uint32_t RAIN_SENSOR_ID = 0x102;
    static constexpr std::uint32_t DOOR_EVENT_ID = 0x103;

    void receive(const CanFrame& frame) {
        auto& bus = ServiceRegistry::getInstance();

        switch (frame.id) {
            case KEY_FOB_COMMAND_ID:
                handleKeyFob(frame, bus);
                break;

            case VEHICLE_SPEED_ID:
                handleVehicleSpeed(frame, bus);
                break;

            case RAIN_SENSOR_ID:
                handleRainSensor(frame, bus);
                break;

            case DOOR_EVENT_ID:
                handleDoorEvent(frame, bus);
                break;

            default:
                break;
        }
    }

private:
    void handleKeyFob(
        const CanFrame& frame,
        ServiceRegistry& bus
    ) {
        if (frame.dlc < 1) {
            return;
        }

        if (frame.data[0] == 1) {
            bus.publish({"key.fob.command", "LOCK"});
        }
        else if (frame.data[0] == 0) {
            bus.publish({"key.fob.command", "UNLOCK"});
        }
    }

    void handleVehicleSpeed(
        const CanFrame& frame,
        ServiceRegistry& bus
    ) {
        if (frame.dlc < 1) {
            return;
        }

        int speed = frame.data[0];

        bus.publish({
            "vehicle.speed",
            std::to_string(speed)
        });
    }

    void handleRainSensor(
        const CanFrame& frame,
        ServiceRegistry& bus
    ) {
        if (frame.dlc < 1) {
            return;
        }

        int intensity = frame.data[0];

        bus.publish({
            "rain.sensor",
            std::to_string(intensity)
        });
    }

    void handleDoorEvent(
        const CanFrame& frame,
        ServiceRegistry& bus
    ) {
        if (frame.dlc < 1) {
            return;
        }

        if (frame.data[0] == 1) {
            bus.publish({"door.open.event", "OPEN"});
        }
        else if (frame.data[0] == 0) {
            bus.publish({"door.open.event", "CLOSED"});
        }
    }
};