# SubMarine ROV — Project Plan

## Overview
PVC-tube-based underwater ROV controlled by Raspberry Pi Zero W V1.1.
Designed for live camera feed + manual control via tether.

---

## Components

### Confirmed
| Component | Qty | Function |
|-----------|-----|----------|
| Raspberry Pi Zero W V1.1 | 1 | Main controller |
| *(add your components here)* | | |

### Proposed (replace with your actual parts)
| Component | Qty | Function |
|-----------|-----|----------|
| Raspberry Pi Camera Module (v1.3 or v2) | 1 | Live video feed |
| DC Motors (300–600RPM, 12V) | 2 | Thrusters (port/starboard) |
| Motor Driver (L298N or DRV8833) | 1 | Drive motors via Pi GPIO |
| Electronic Speed Controller (ESC) | 2 | Motor speed control |
| LiPo Battery (11.1V–14.8V, 2200mAh+) | 1 | Main power |
| Voltage Regulator (12V→5V, 3A) | 1 | Safe power for Pi Zero |
| Ethernet-over-USB adapter (RNDIS) | 1 | Tether data link |
| LED strip (12V, waterproof) | 1 | Underwater lighting |
| PVC tube (Ø100mm×400mm) | 1 | Main pressure hull |
| PVC end caps + O-rings | 2 | Waterproof seals |
| Cable gland (PG9/PG11) | 2–4 | Wire passthrough |
| 4-core shielded cable (10m+) | 1 | Tether (power + data) |

---

## Wiring Architecture

```
                     ┌──────────────────────┐
                     │     LiPo Battery      │
                     │     11.1V–14.8V       │
                     └────────┬─────────────┘
                              │
                    ┌─────────┴─────────┐
                    │  Voltage Regulator │
                    │    12V → 5V 3A    │
                    └─────────┬─────────┘
                              │ 5V
                    ┌─────────┴──────────┐
                    │  Raspberry Pi Zero │
                    │      W V1.1        │
                    └──┬───┬───┬───┬─────┘
                       │   │   │   │
              ┌────────┘   │   │   └──────────┐
              │            │   │              │
        ┌─────┴─────┐     │   │     ┌────────┴────────┐
        │  Motor    │     │   │     │  USB → Ethernet  │
        │  Driver   │     │   │     │  (RNDIS tether)  │
        │ (GPIO)    │     │   │     └────────┬─────────┘
        └──┬───┬────┘     │   │              │
           │   │         CSI │            Tether (RJ45)
      ┌────┘   └────┐   │   │
   ┌──┴──┐      ┌───┴──┐ │   └──────────────────┐
   │ESC1 │      │ ESC2 │ │                      │
   └──┬──┘      └──┬───┘ │                 ┌────┴─────┐
      │            │     │                 │ LED Strip │
   ┌──┴──┐     ┌───┴──┐ │                 │ (12V)    │
   │Mtr1 │     │ Mtr2 │ │                 └──────────┘
   └─────┘     └──────┘ │
                    ┌────┴────┐
                    │ Camera  │
                    │ Module  │
                    └─────────┘
```

### Pin Mapping (RPi Zero GPIO)
| GPIO Pin | Connected to         |
|----------|----------------------|
| GPIO17   | Motor Driver IN1     |
| GPIO18   | Motor Driver IN2     |
| GPIO22   | Motor Driver IN3     |
| GPIO23   | Motor Driver IN4     |
| GPIO27   | Motor Driver ENA     |
| GPIO24   | Motor Driver ENB     |
| 5V Pin   | Voltage Regulator    |
| GND      | Common ground        |

### Power Budget
| Component | Voltage | Current (est.) |
|-----------|---------|---------------|
| Pi Zero W | 5V      | ~200mA |
| Camera    | 3.3V    | ~250mA |
| Motor Driver | 5V   | ~50mA (logic) |
| 2× Motors | 12V     | ~1–2A each |
| LED Strip | 12V     | ~500mA |
| **Total** | 12V     | **~3–4A** |

---

## PVC Tube Layout (Cross-section)

```
  ┌──────────────────────────────────────────┐
  │  End Cap (rear)        PVC Tube          │  End Cap (front)
  │  ┌─────┐  ┌────────┐  ┌──────┐  ┌────┐  │
  │  │Bat  │  │ Pi Zero│  │Motor │  │Cam │  │
  │  │tery │  │+Reg    │  │Driver│  │+LED│  │
  │  │     │  │        │  │      │  │    │  │
  │  └─────┘  └────────┘  └──────┘  └────┘  │
  │     ^            ^           ^            │
  │   Cable          USB        Ribbon       │
  │   Gland        Tether       Cable        │
  └──────────────────────────────────────────┘
```

**Ballast:** Battery at rear for weight balance.
**Sealing:** O-ring grooves in end caps + silicone grease.
**Cable glands:** Epoxy-sealed on rear cap for tether.

---

## Next Steps

1. **Replace placeholder components** with your actual parts in the table above
2. **Test electronics outside tube** — bench test Pi, motors, camera
3. **Code the control loop** — Python script for motor control + video streaming
4. **Dry-fit in PVC tube** before final sealing
5. **Water test** — submerged test in bucket/tub
6. **First dive** — pool/calm water test

---

## Software Stack (proposed)
- **OS:** Raspberry Pi OS Lite (Bullseye)
- **Language:** Python 3
- **Camera:** picamera2 + Flask or MJPEG streamer
- **Motor control:** RPi.GPIO + PWM on L298N
- **Control interface:** Web UI over tether (flask + joystick)
- **Communication:** SSH over USB Ethernet/RNDIS

---

*This plan is a living document — update as you finalize parts and wiring.*
