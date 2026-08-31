#include <gtest/gtest.h>

#include "DoorLockService.h"
#include "ServiceRegistry.h"
#include "WiperService.h"
#include "CanGateway.h"

class VehicleServiceTest : public ::testing::Test {
protected:
    void TearDown() override {
        ServiceRegistry::getInstance().reset();
    }
};

TEST_F(VehicleServiceTest, AutoLocksAbove10MPH) {
    DoorLockService doorLock;
    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"key.fob.command", "UNLOCK"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());

    bus.publish({"vehicle.speed", "25.0"});
    bus.waitForIdle();

    EXPECT_TRUE(doorLock.isLocked());
}

TEST_F(VehicleServiceTest, DoesNotAutoLockAt10MPHOrBelow) {
    DoorLockService doorLock;
    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"key.fob.command", "UNLOCK"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());

    bus.publish({"vehicle.speed", "10.0"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());
}

TEST_F(VehicleServiceTest, RejectsNegativeVehicleSpeed) {
    DoorLockService doorLock;
    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"key.fob.command", "UNLOCK"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());

    bus.publish({"vehicle.speed", "-5.0"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());
}

TEST_F(VehicleServiceTest, SetsLowModeForLightRain) {
    WiperService wipers;
    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"rain.sensor", "30"});
    bus.waitForIdle();

    EXPECT_EQ(wipers.getMode(), "LOW");
}

TEST_F(VehicleServiceTest, SetsHighModeForHeavyRain) {
    WiperService wipers;
    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"rain.sensor", "80"});
    bus.waitForIdle();

    EXPECT_EQ(wipers.getMode(), "HIGH");
}

TEST_F(VehicleServiceTest, RejectsOutOfRangeRainIntensity) {
    WiperService wipers;
    auto& bus = ServiceRegistry::getInstance();

    EXPECT_EQ(wipers.getMode(), "OFF");

    bus.publish({"rain.sensor", "150"});
    bus.waitForIdle();

    EXPECT_EQ(wipers.getMode(), "OFF");
}

TEST_F(VehicleServiceTest, ProcessesManyMessages) {
    auto& bus = ServiceRegistry::getInstance();

    int received = 0;

    bus.subscribe(
        "stress.test",
        "TestSubscriber",
        [&received](const Message&) {
            ++received;
        }
    );

    for (int i = 0; i < 1000; ++i) {
        bus.publish({"stress.test", std::to_string(i)});
    }

    bus.waitForIdle();

    EXPECT_EQ(received, 1000);
}

TEST_F(VehicleServiceTest, CanSpeedFrameTriggersAutoLock) {
    DoorLockService doorLock;
    CanGateway gateway;

    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"key.fob.command", "UNLOCK"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());

    CanFrame speedFrame;
    speedFrame.id = CanGateway::VEHICLE_SPEED_ID;
    speedFrame.data[0] = 25;
    speedFrame.dlc = 1;

    gateway.receive(speedFrame);
    bus.waitForIdle();

    EXPECT_TRUE(doorLock.isLocked());
}

TEST_F(VehicleServiceTest, CanRainFrameSetsWipersHigh) {
    WiperService wipers;
    CanGateway gateway;

    auto& bus = ServiceRegistry::getInstance();

    CanFrame rainFrame;
    rainFrame.id = CanGateway::RAIN_SENSOR_ID;
    rainFrame.data[0] = 80;
    rainFrame.dlc = 1;

    gateway.receive(rainFrame);
    bus.waitForIdle();

    EXPECT_EQ(wipers.getMode(), "HIGH");
}

TEST_F(VehicleServiceTest, RejectsMalformedCanFrameWithMissingPayload) {
    DoorLockService doorLock;
    CanGateway gateway;

    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"key.fob.command", "UNLOCK"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());

    CanFrame malformedFrame;
    malformedFrame.id = CanGateway::VEHICLE_SPEED_ID;
    malformedFrame.dlc = 0;

    gateway.receive(malformedFrame);
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());
}

TEST_F(VehicleServiceTest, IgnoresUnknownCanId) {
    DoorLockService doorLock;
    WiperService wipers;
    CanGateway gateway;

    auto& bus = ServiceRegistry::getInstance();

    bus.publish({"key.fob.command", "UNLOCK"});
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());
    EXPECT_EQ(wipers.getMode(), "OFF");

    CanFrame unknownFrame;
    unknownFrame.id = 0x7FF;
    unknownFrame.data[0] = 99;
    unknownFrame.dlc = 1;

    gateway.receive(unknownFrame);
    bus.waitForIdle();

    EXPECT_FALSE(doorLock.isLocked());
    EXPECT_EQ(wipers.getMode(), "OFF");
}

TEST_F(VehicleServiceTest, RejectsInvalidRainValueFromCanFrame) {
    WiperService wipers;
    CanGateway gateway;

    auto& bus = ServiceRegistry::getInstance();

    EXPECT_EQ(wipers.getMode(), "OFF");

    CanFrame invalidRainFrame;
    invalidRainFrame.id = CanGateway::RAIN_SENSOR_ID;
    invalidRainFrame.data[0] = 150;
    invalidRainFrame.dlc = 1;

    gateway.receive(invalidRainFrame);
    bus.waitForIdle();

    EXPECT_EQ(wipers.getMode(), "OFF");
}