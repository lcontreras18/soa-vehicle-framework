# SDV Body Control — Service-Oriented Architecture (SoA) Framework

A lightweight C++17 implementation of a **Service-Oriented Architecture** for automotive Body Control Module (BCM) software, inspired by Software-Defined Vehicle (SDV) principles.

Built as a portfolio project targeting Karma Automotive's SDV Body Control domain.

---

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                   ServiceRegistry                   │
│   (Pub/Sub Message Bus — singleton, thread-safe)    │
└────────────┬────────────────────────────────────────┘
             │  publish / subscribe
   ┌─────────┼──────────────────────────┐
   ▼         ▼                          ▼
DoorLockService   InteriorLightingService   WiperService
   │                    ▲                      ▲
   └── publishes ───────┘                      │
       door.lock.status             rain.sensor topic
```

Each **service** is a self-contained unit that:
- Registers itself with the `ServiceRegistry` on startup
- Subscribes to the topics it cares about
- Publishes events when its state changes

This mirrors how real SDV architectures decompose vehicle functions into loosely-coupled services.

---

## Services

| Service | Subscribes To | Publishes |
|---|---|---|
| `DoorLockService` | `key.fob.command`, `vehicle.speed` | `door.lock.status` |
| `InteriorLightingService` | `door.lock.status`, `door.open.event` | `interior.light.status` |
| `WiperService` | `rain.sensor` | `wiper.status` |

---

## Key Concepts Demonstrated

- **Service-Oriented Architecture (SoA)** — services communicate via topics, not direct calls
- **Pub/Sub message bus** — decoupled event-driven communication
- **State machines** — e.g. DoorLockService manages LOCKED/UNLOCKED state transitions
- **Cross-service reactivity** — unlocking doors automatically triggers interior lighting
- **Thread safety** — `std::recursive_mutex` allows services to publish from within callbacks
- **Automotive domain logic** — speed-based auto-lock, rain sensor wiper control

---

## Build & Run

### Requirements
- C++17 compiler (g++ 13+)
- CMake 3.14+

### Steps
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./soa_vehicle
```

---

## Sample Output

```
=== Karma SDV Body Control SoA Framework ===

[Registry] Service registered: DoorLockService
[Registry] Service registered: InteriorLightingService
[Registry] Service registered: WiperService

========== Scenario 1: Key Fob UNLOCK ==========
[Bus] Publishing topic='key.fob.command' payload='UNLOCK'
[DoorLockService] Doors are now: UNLOCKED
[InteriorLighting] Light ON (Door unlocked)

========== Scenario 4: Vehicle Reaches 25 mph (Auto-Lock) ==========
[DoorLockService] Speed > 10 mph — auto-locking doors
[DoorLockService] Doors are now: LOCKED
[InteriorLighting] Light OFF (Door locked)
```

---

## Extending the Framework

To add a new service (e.g. `SeatBeltService`):
1. Create `include/SeatBeltService.h`
2. In the constructor, call `registry.registerService(...)` and `registry.subscribe(...)`
3. Instantiate it in `main.cpp`

No changes needed to the registry or other services — that's the power of SoA.

---

## Relevance to SDV / BCM Engineering

In real production BCM software (AUTOSAR-compliant or otherwise), services communicate over CAN/LIN/Ethernet using similar pub/sub patterns (e.g. SOME/IP, DDS). This project demonstrates the same architectural thinking at a framework level, in pure C++ without hardware dependencies.
