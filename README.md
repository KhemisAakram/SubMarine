<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://img.shields.io/badge/status-active--development-blue?style=flat-square">
  <img alt="Status" src="https://img.shields.io/badge/status-active--development-blue?style=flat-square">
</picture>
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://img.shields.io/badge/platform-Raspberry%20Pi%20Zero%20W-brightgreen?style=flat-square&logo=raspberry-pi">
  <img alt="Platform" src="https://img.shields.io/badge/platform-Raspberry%20Pi%20Zero%20W-brightgreen?style=flat-square&logo=raspberry-pi">
</picture>
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://img.shields.io/badge/language-Python%203-yellow?style=flat-square&logo=python">
  <img alt="Language" src="https://img.shields.io/badge/language-Python%203-yellow?style=flat-square&logo=python">
</picture>

# SubMarine ROV

A PVC-tube-based underwater Remotely Operated Vehicle (ROV) controlled by a **Raspberry Pi Zero W V1.1**, featuring real-time object detection, pressure sensing, and syringe-actuated buoyancy control.

> **⚠️ WARNING:** This project is in **active development**. The Mini L298N motor drivers are rated at 2A per channel — the 34900 motors may exceed this under load. A BTS7960 upgrade path is documented.

---

## Features

- **Live video stream** from OV5647 camera over tether (USB-Ethernet)
- **Real-time object detection** via TensorFlow Lite with COCO-SSD model
- **Differential thrust control** — Rear motor for forward/reverse, Side motor for left/right
- **Depth & pressure sensing** — MPX5010DP pressure sensor read through ADS1115 16-bit ADC
- **Obstacle avoidance** — VL53L0X Time-of-Flight distance sensor (up to 2m)
- **Active buoyancy control** — N20 micro gear motor driving dual 20ml syringes to adjust ballast
- **Web-based control UI** — Flask web interface with joystick and live video feed
- **Tether communication** — USB-Ethernet (RNDIS) for reliable low-latency control

---

## Components

| Component | Qty | Purpose |
|-----------|-----|---------|
| Raspberry Pi Zero W V1.1 | 1 | Main controller |
| OV5647 Camera Module | 1 | Video + COCO-SSD object detection |
| 34900 DC Motor (12V) | 2 | Rear (Fwd/Rev) & Side (Left/Right) propulsion |
| Mini L298N Motor Driver | 2 | H-bridge control for all motors |
| MPX5010DP + ADS1115 | 1 | Pressure/depth measurement |
| VL53L0X V2 ToF Sensor | 1 | Obstacle distance sensing |
| N20 Micro Gear Motor | 1 | Syringe ballast actuation |
| 20ml Syringe | 2 | Buoyancy control (push/pull water) |
| 18650 3.7V 2200mAh ×3 | 3 | 3S battery pack (11.1V nominal) |
| LM2596 Buck Converter | 1 | 12V→5V 3A for Pi Zero |

Full details with prices and Algerian store links → [PriceComponents.md](PriceComponents.md)

---

## Project Structure

```
SubMarine/
├── README.md                  ← You are here
├── ProjectPlan.md             # High-level project overview & next steps
├── WiringPlan.md              # Wiring architecture, I2C bus, GPIO mapping
├── PowerManagement.md         # Power tree, budget, runtime, protection
├── PriceComponents.md         # Component prices + Algerian store info
└── src/
    ├── pins.h                 # GPIO pin definitions (C header)
    ├── pins.py                # GPIO pin definitions (Python module)
    └── CodingRef.md           # Full technical reference for coding
```

---

## GPIO Map (BCM)

| GPIO | Connected To | Function |
|------|-------------|----------|
| 17 | L298N #1 IN1 | Rear Motor Dir A |
| 18 | L298N #1 IN2 | Rear Motor Dir B |
| 27 | L298N #1 ENA | Rear Motor PWM |
| 22 | L298N #2 IN3 | Side Motor Dir A |
| 23 | L298N #2 IN4 | Side Motor Dir B |
| 24 | L298N #2 ENB | Side Motor PWM |
| 5 | L298N #1 IN3 | Ballast Dir A |
| 6 | L298N #1 IN4 | Ballast Dir B |
| 19 | L298N #1 ENB | Ballast PWM |
| 2 | — | I2C SDA (ADS1115, VL53L0X) |
| 3 | — | I2C SCL (ADS1115, VL53L0X) |

Full wiring details → [WiringPlan.md](WiringPlan.md)  
Header definitions → [src/pins.h](src/pins.h) · [src/pins.py](src/pins.py)

---

## Getting Started

### Hardware Setup

1. Assemble the 3S battery pack with BMS protection
2. Wire components per [WiringPlan.md](WiringPlan.md)
3. Bench-test all electronics outside the PVC tube first
4. Dry-fit inside tube, then seal with O-rings and cable glands

### Software Setup

```bash
# Flash Raspberry Pi OS Lite (Bookworm) to microSD
# Enable I2C, Camera, and SSH on first boot

# Install dependencies
sudo apt update
sudo apt install -y python3-pip python3-picamera2 python3-opencv i2c-tools pigpio python3-pigpio

# Install Python packages
pip install flask flask-socketio tflite-runtime \
            adafruit-circuitpython-ads1115 \
            adafruit-circuitpython-vl53l0x \
            numpy RPi.GPIO

# Clone the repo on the Pi
git clone https://github.com/KhemisAakram/SubMarine.git
```

### Power Budget

| Scenario | Current Draw | Est. Runtime (3× 18650 2200mAh) |
|----------|-------------|-------------------------------|
| Full throttle + LED | ~9A | ~2 min |
| Cruise + LED | ~4A | ~5 min |
| Idle (sensors only) | ~0.5A | ~38 min |

Full power analysis → [PowerManagement.md](PowerManagement.md)

---

## Software Stack

| Layer | Technology |
|-------|-----------|
| OS | Raspberry Pi OS Lite (Bookworm) |
| Language | Python 3 |
| Camera | picamera2 + OpenCV |
| Object Detection | TensorFlow Lite (COCO-SSD MobileNet) |
| Motor Control | RPi.GPIO / pigpio (PWM 1 kHz) |
| Sensors (I2C) | adafruit-circuitpython (ADS1115, VL53L0X) |
| Web Interface | Flask + MJPEG streamer |
| Communication | USB-Ethernet (RNDIS) over tether |

Coding reference → [src/CodingRef.md](src/CodingRef.md)

---

## Roadmap

- [x] Component selection & wiring plan
- [x] Power management & budget analysis
- [x] Algerian store sourcing & pricing
- [x] GPIO pin definitions (C + Python headers)
- [ ] Bench test electronics outside tube
- [ ] I2C sensor calibration
- [ ] Motor control code & PID for depth hold
- [ ] COCO-SSD deployment on Pi Zero
- [ ] Web UI with joystick + video stream
- [ ] Water test (bucket → pool)

---

## Built With

- **Raspberry Pi Zero W** — [raspberrypi.com](https://www.raspberrypi.com)
- **TensorFlow Lite** — [tensorflow.org/lite](https://www.tensorflow.org/lite)
- **picamera2** — [github.com/raspberrypi/picamera2](https://github.com/raspberrypi/picamera2)

---

**Author:** Khemis Akram  
**License:** MIT  
**Repo:** [github.com/KhemisAakram/SubMarine](https://github.com/KhemisAakram/SubMarine)
