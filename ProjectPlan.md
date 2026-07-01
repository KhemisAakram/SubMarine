# SubMarine ROV — Project Plan

## Overview
PVC-tube-based underwater ROV controlled by Raspberry Pi Zero W V1.1.
Camera-based object detection (COCO-SSD), pressure/depth sensing, and buoyancy control via syringe ballast system.

---

## Components

| Component | Qty | Function |
|-----------|-----|----------|
| Raspberry Pi Zero W V1.1 | 1 | Main controller |
| OV5647 Camera Module | 1 | Object detection via COCO-SSD |
| 34900 DC Motor 12V | 2 | Propulsion: rear (Fwd/Rev) + side (Left/Right) |
| BTS7960 H-bridge Motor Driver | 2 | Drive each 34900 motor (43A peak, 12V) |
| MPX5010DP + ADS1115 | 1 | Pressure / depth measurement (0–10kPa ≈ 1m H₂O) |
| VL53L0X V2 (ToF) | 1 | Obstacle distance sensing (up to 2m) |
| N20 Micro Gear Motor | 1 | Syringe ballast actuation |
| 20ml Syringe | 2 | Ballast — push/pull water for buoyancy |
| 18650 3.7V 2200mAh | 3 | 3S pack = 11.1V nominal main power |
| Voltage Regulator (12V→5V, 3A) | 1 | Safe 5V for Pi Zero |
| Ethernet-over-USB adapter (RNDIS) | 1 | Tether data link |
| LED strip (12V) | 1 | Underwater lighting |
| PVC tube (Ø100mm×400mm) | 1 | Main pressure hull |
| PVC end caps + O-rings | 2 | Waterproof seals |
| Cable gland (PG9/PG11) | 2–4 | Wire passthrough |
| 6+ core shielded cable (10m+) | 1 | Tether (power + data) |

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
              ┌────────┘   │   │   │   └──────────┐
              │            │   │   │              │
        ┌─────┴─────┐     │   │   │     ┌────────┴────────┐
        │ BTS7960   │     │   │   │     │  USB→Ethernet   │
        │ (Rear)    │     │   │   │     │  (RNDIS tether) │
        │ GPIO      │     │   │   │     └────────┬─────────┘
        └──┬───┬────┘     │   │   │              │
           │   │          │   │   │            Tether
      ┌────┘   └────┐    │   │   │
   ┌──┴──┐      ┌───┴──┐ │   │   └──────────────────┐
   │Mtr1 │      │ Mtr2 │ │   │                      │
   │Rear │      │ Side │ │   │                 ┌────┴──────┐
   │Fwd/R│      │ L/R  │ │   │                 │ LED Strip │
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

                    ┌───────────────────────┐
                    │ N20 Motor + Syringe   │
                    │ (Ballast Control)     │
                    │ GPIO (via motor drv)  │
                    └───────────────────────┘
```

### I2C Bus (for sensors)
| Sensor | Address (I²C) | Pi Pin |
|--------|--------------|--------|
| ADS1115 (MPX5010DP) | 0x48 | GPIO2 (SDA), GPIO3 (SCL) |
| VL53L0X V2 | 0x29 | GPIO2 (SDA), GPIO3 (SCL) |

### Pin Mapping (RPi Zero GPIO)
| GPIO Pin | Connected to |
|----------|-------------|
| GPIO17 | BTS7960 RPWM (Rear Motor) |
| GPIO18 | BTS7960 LPWM (Rear Motor) |
| GPIO22 | BTS7960 RPWM (Side Motor) |
| GPIO23 | BTS7960 LPWM (Side Motor) |
| GPIO27 | BTS7960 R_EN (Rear) |
| GPIO24 | BTS7960 L_EN (Rear) |
| GPIO5 | BTS7960 R_EN (Side) |
| GPIO6 | BTS7960 L_EN (Side) |
| GPIO12 | N20 Motor PWM (Syringe ballast) |
| GPIO13 | N20 Motor DIR (Syringe ballast) |
| GPIO2 | I2C SDA |
| GPIO3 | I2C SCL |
| CSI | OV5647 Camera ribbon |
| 5V Pin | Voltage Regulator (5V) |
| GND | Common ground |

### Power Budget
| Component | Voltage | Current (est.) |
|-----------|---------|---------------|
| Pi Zero W | 5V | ~200mA |
| OV5647 Camera | 3.3V | ~250mA |
| BTS7960 (logic) | 5V | ~50mA total |
| 2× 34900 Motors | 12V | ~2–4A each (under load) |
| N20 Motor | 5V | ~100mA |
| MPX5010DP + ADS1115 | 5V | ~10mA |
| VL53L0X V2 | 3.3V | ~20mA |
| LED Strip | 12V | ~500mA |
| **Total (max)** | **11.1V** | **~7–9A** |

> **Note:** 3× 18650 (2200mAh each) ≈ 66Wh. At 9A draw ≈ 7–8 min runtime at full load. Consider upgrading to higher capacity or adding more cells in parallel.

---

## PVC Tube Layout (Cross-section)

```
  ┌──────────────────────────────────────────────┐
  │  Rear Cap               PVC Tube          Front Cap │
  │  ┌────────┐ ┌────────┐ ┌──────┐ ┌─────────┐ │
  │  │Battery │ │ PiZero │ │Sensors│ │ Camera  │ │
  │  │3×18650 │ │ +Reg   │ │+Mtr  │ │ +LED    │ │
  │  │ 3S     │ │ +BTS   │ │Drv   │ │         │ │
  │  └────────┘ └────────┘ └──────┘ └─────────┘ │
  │      ^           ^         ^          ^       │
  │  Cable Gland   USB     I2C/GPIO   CSI Ribbon │
  │  (Tether)     Tether    Wires      Cable     │
  └──────────────────────────────────────────────┘
```

**Ballast:** Syringe system — N20 motor pushes/pulls 20ml syringes to adjust buoyancy.
**Sealing:** O-ring grooves in end caps + silicone grease.
**Cable glands:** Epoxy-sealed on rear cap for tether.

---

## Buoyancy Control (Syringe Ballast)

```
  N20 Motor ← GPIO12(PWM) + GPIO13(DIR)
      │
      ├── Rack & pinion / leadscrew
      │
  ┌───┴───┐     ┌───┴───┐
  │ Syr 1 │     │ Syr 2 │
  │ 20ml  │─────│ 20ml  │
  └───────┘     └───────┘
      │              │
  ────┴──────────────┴──── Water intake/outtake
```

- N20 motor drives both syringes in parallel
- Push = expel water → lighter → ascend
- Pull = intake water → heavier → descend
- Total displacement: 40ml of water ≈ 40g buoyancy change

---

## Next Steps

1. **Bench test electronics** — Pi Zero, motors, sensors outside tube
2. **I²C sensor setup** — ADS1115 + MPX5010DP, VL53L0X
3. **Motor control code** — PWM via BTS7960 for differential thrust
4. **Camera + COCO-SSD** — TensorFlow Lite object detection pipeline
5. **Syringe ballast test** — N20 + syringe mechanism
6. **Dry-fit in PVC tube** — layout all components
7. **Water test** — bucket test for buoyancy + sealing
8. **First dive** — pool/calm water

---

## Software Stack
- **OS:** Raspberry Pi OS Lite (Bookworm)
- **Language:** Python 3
- **Camera:** picamera2 + TensorFlow Lite (COCO-SSD)
- **I²C Sensors:** smbus2 / adafruit-circuitpython libraries
- **Motor control:** RPi.GPIO + PWM
- **Control interface:** Web UI over tether (Flask + joystick)
- **Communication:** SSH over USB Ethernet/RNDIS

---

*This plan is a living document — update as you finalize parts and wiring.*
