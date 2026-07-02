# SubMarine ROV — Wiring Plan

Wiring architecture, I2C bus, GPIO pin mapping, and power budget for the SubMarine ROV.

---

## Wiring Architecture

```
                      ┌──────────────────────────┐
                      │   3× 18650 (3S 11.1V)     │
                      │       2200mAh             │
                      └────────┬─────────────────┘
                               │ 11.1V
                     ┌─────────┴──────────┐
                     │  Voltage Regulator  │
                     │    12V→5V 3A       │
                     └─────────┬──────────┘
                               │ 5V
                     ┌─────────┴──────────────┐
                     │   Raspberry Pi Zero W  │
                     │        V1.1            │
                     └──┬───┬───┬───┬───┬─────┘
                        │   │   │   │   │
              ┌─────────┘   │   │   │   └──────────┐
              │             │   │   │              │
        ┌─────┴─────┐      │   │   │     ┌────────┴────────┐
        │ L298N #1  │      │   │   │     │  USB→Ethernet   │
        │ (Rear+Bal)│      │   │   │     │  (RNDIS tether) │
        │ GPIO      │      │   │   │     └────────┬─────────┘
        └──┬───┬────┘      │   │   │              │
           │   │           │   │   │            Tether
      ┌────┘   └────┐     │   │   │
   ┌──┴──┐      ┌───┴──┐ │   │   └──────────────────┐
   │Mtr1 │      │ N20  │ │   │                      │
   │Rear │      │Balast│ │   │                 ┌────┴──────┐
   │Fwd/R│      │ Syr  │ │   │                 │ LED Strip │
   └─────┘      └──────┘ │   │                 │ (12V)     │
                          │   │                 └───────────┘
                    ┌─────┴───┴──────┐
                    │   I2C Bus      │
                    │  SDA ── GPIO2  │
                    │  SCL ── GPIO3  │
                    └──┬──────┬──────┘
                       │      │
              ┌────────┴┐  ┌──┴────────┐
              │ ADS1115 │  │ VL53L0X   │
              │ +MPX5010│  │ V2 (ToF)  │
              └─────────┘  └───────────┘

              (L298N #2 — Side motor — same layout as L298N #1 but Ch B only)
```

> **Note:** Mini L298N jumpers should be removed on ENA/ENB to enable PWM speed control. Connect 5V logic compatible with Pi GPIO.

---

## L298N Motor Driver Pinout

Each Mini L298N has 2 H-bridge channels (Ch A: IN1/IN2/ENA/OUT1/OUT2, Ch B: IN3/IN4/ENB/OUT3/OUT4):
- **IN1, IN2 / IN3, IN4** — Direction control (HIGH/LOW)
- **ENA / ENB** — PWM speed (remove jumper, connect to GPIO PWM)
- **+12V, GND** — Motor power supply (12V rail)
- **+5V** — Output 5V (if jumper on; or feed 5V logic from Pi)

### L298N #1 — Rear Motor (Ch A) + Ballast (Ch B)

#### Channel A — Rear 34900 Motor (Forward / Reverse)
| L298N Pin | Connect to | Pi GPIO |
|-----------|-----------|---------|
| IN1 | GPIO17 | Direction A |
| IN2 | GPIO18 | Direction B |
| ENA | GPIO27 | PWM speed |
| +12V | 12V Rail | Battery |
| GND | Common GND | Battery - |
| Out1/Out2 | Rear 34900 Motor | |

#### Channel B — N20 Ballast Motor (Syringe push/pull)
| L298N Pin | Connect to | Pi GPIO |
|-----------|-----------|---------|
| IN3 | GPIO5 | Direction A |
| IN4 | GPIO6 | Direction B |
| ENB | GPIO19 | PWM speed |
| +12V | 12V Rail | Battery (shared with Ch A) |
| GND | Common GND | Battery - |
| Out3/Out4 | N20 Ballast Motor | |

### L298N #2 — Side Motor (Ch B only)

#### Channel B — Side 34900 Motor (Left / Right)
| L298N Pin | Connect to | Pi GPIO |
|-----------|-----------|---------|
| IN3 | GPIO22 | Direction A |
| IN4 | GPIO23 | Direction B |
| ENB | GPIO24 | PWM speed |
| +12V | 12V Rail | Battery |
| GND | Common GND | Battery - |
| Out3/Out4 | Side 34900 Motor | |

---

## I2C Bus
| Sensor | Address (I2C) | Pi Pin |
|--------|--------------|--------|
| ADS1115 (MPX5010DP) | 0x48 | GPIO2 (SDA), GPIO3 (SCL) |
| VL53L0X V2 | 0x29 | GPIO2 (SDA), GPIO3 (SCL) |

---

## Pin Mapping (RPi Zero GPIO)
| GPIO Pin | Connected to |
|----------|-------------|
| GPIO17 | L298N #1 IN1 (Rear Motor Dir A) |
| GPIO18 | L298N #1 IN2 (Rear Motor Dir B) |
| GPIO27 | L298N #1 ENA (Rear Motor PWM) |
| GPIO5 | L298N #1 IN3 (Ballast Dir A) |
| GPIO6 | L298N #1 IN4 (Ballast Dir B) |
| GPIO19 | L298N #1 ENB (Ballast PWM) |
| GPIO22 | L298N #2 IN3 (Side Motor Dir A) |
| GPIO23 | L298N #2 IN4 (Side Motor Dir B) |
| GPIO24 | L298N #2 ENB (Side Motor PWM) |
| GPIO2 | I2C SDA |
| GPIO3 | I2C SCL |
| CSI | OV5647 Camera ribbon |
| 5V Pin | Voltage Regulator (5V out) |
| GND | Common ground |

Standalone pin header files are also available at:
- [src/pins.h](src/pins.h) — C/C++ header
- [src/pins.py](src/pins.py) — Python module

---

## Power Budget
| Component | Voltage | Current (est.) |
|-----------|---------|---------------|
| Pi Zero W | 5V | ~200mA |
| OV5647 Camera | 3.3V | ~250mA |
| L298N Logic (×2) | 5V | ~50mA total |
| 2× 34900 Motors | 12V | ~2–4A each (under load) |
| N20 Motor | 5V | ~100mA |
| MPX5010DP + ADS1115 | 5V | ~10mA |
| VL53L0X V2 | 3.3V | ~20mA |
| LED Strip | 12V | ~500mA |
| **Total (max)** | **11.1V** | **~7–9A** |

> **⚠️ L298N Limit:** Mini L298N max 2A per channel. 34900 motors can draw 3–5A at stall. For bench testing without load this should be fine. For underwater operation under load, add heatsinks to L298N or upgrade to BTS7960 (43A peak).

> **Note:** 3× 18650 (2200mAh each) ≈ 66Wh. At 9A draw ≈ 7–8 min runtime at full load.

---

*This wiring plan is a living document — update as connections change.*
