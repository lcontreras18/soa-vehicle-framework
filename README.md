# SDV Body Control — Service-Oriented Architecture Framework

---

## Services

| Service | Subscribes To | Publishes |
|---|---|---|
| `DoorLockService` | `key.fob.command`, `vehicle.speed` | `door.lock.status` |
| `InteriorLightingService` | `door.lock.status`, `door.open.event` | `interior.light.status` |
| `WiperService` | `rain.sensor` | `wiper.status` |

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

---
