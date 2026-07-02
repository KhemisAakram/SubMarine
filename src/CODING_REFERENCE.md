# SubMarine ROV — Coding Reference

All technical details needed to write control software for the Raspberry Pi Zero W V1.1.

---

## 1. GPIO Pin Mapping

See [pins.h](pins.h) (C) and [pins.py](pins.py) (Python) for the definitive pin definitions.

| GPIO (BCM) | Physical Pin | Identifier | Connected To |
|------------|-------------|------------|-------------|
| 2 | 3 | `I2C_SDA` | Shared: ADS1115 (0x48), VL53L0X (0x29) |
| 3 | 5 | `I2C_SCL` | Shareed: ADS1115 (0x48), VL53L0X (0x29) |
| 5 | 29 | `BALL_IN3` | L298N #1 IN3 — Ballast Dir A |
| 6 | 31 | `BALL_IN4` | L298N #1 IN4 — Ballast Dir B |
| 17 | 11 | `REAR_IN1` | L298N #1 IN1 — Rear Motor Dir A |
| 18 | 12 | `REAR_IN2` | L298N #1 IN2 — Rear Motor Dir B |
| 19 | 35 | `BALL_ENB` | L298N #1 ENB — Ballast PWM |
| 22 | 15 | `SIDE_IN3` | L298N #2 IN3 — Side Motor Dir A |
| 23 | 16 | `SIDE_IN4` | L298N #2 IN4 — Side Motor Dir B |
| 24 | 18 | `SIDE_ENB` | L298N #2 ENB — Side Motor PWM |
| 27 | 13 | `REAR_ENA` | L298N #1 ENA — Rear Motor PWM |

**5V Pin (Physical 2, 4):** Power from LM2596 buck converter  
**GND Pin (Physical 6, 9, 14, etc.):** Common ground, star-connected

---

## 2. Motor Control — L298N Logic

### 2.1 34900 Motors (Rear & Side)

Each Mini L298N channel drives one 34900 DC motor at 12V.

```
IN1 (or IN3)   IN2 (or IN4)   ENA (or ENB)   Motor Behavior
─────────────────────────────────────────────────────────────
      0              0             X           Brake (coast)
      1              0             1           Forward (full speed)
      1              0           PWM           Forward (variable speed)
      0              1             1           Reverse (full speed)
      0              1           PWM           Reverse (variable speed)
      1              1             X           Brake (active)
```

**Rear Motor (L298N #1 Ch A):** GPIO17 (IN1), GPIO18 (IN2), GPIO27 (ENA-PWM)  
**Side Motor (L298N #2 Ch B):** GPIO22 (IN3), GPIO23 (IN4), GPIO24 (ENB-PWM)

### 2.2 Ballast Motor — N20 (L298N #1 Ch B)

```
IN3   IN4   ENB   Behavior
─────────────────────────────
 1     0     1     Expel water → ascend (full speed)
 1     0    PWM    Expel water → ascend (variable speed)
 0     1     1     Intake water → descend (full speed)
 0     1    PWM    Intake water → descend (variable speed)
 0     0     0     Hold position (brake)
 1     1     X     Brake (active)
```

**Ballast (L298N #1 Ch B):** GPIO5 (IN3), GPIO6 (IN4), GPIO19 (ENB-PWM)

### 2.3 PWM Configuration

| Parameter | Value |
|-----------|-------|
| Frequency | 1000 Hz (1 kHz) — standard for DC motors |
| Range | 0–100 (duty cycle %) |
| GPIO library | `RPi.GPIO` with `pwm.ChangeDutyCycle(dc)` |
| Alternative | `pigpio` for hardware-timed PWM (more stable) |

> **pigpio** is recommended for the Pi Zero W as it uses the DMA/PWM hardware, producing cleaner signals and lower CPU usage. Install: `sudo apt install pigpio python3-pigpio`.

---

## 3. I2C Sensors

Enable I2C: `sudo raspi-config` → Interface Options → I2C → Enable

### 3.1 ADS1115 + MPX5010DP (Pressure / Depth)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x48 (ADDR pin to GND) |
| ADC Resolution | 16-bit |
| Input Channel | A0 (differential: A0 = signal+, A1 = GND) |
| PGA Gain | ±4.096V (default) — covers MPX5010DP output (0.2–4.7V) |
| Data Rate | 128 SPS (default, good balance of noise vs speed) |
| MPX5010DP Range | 0–10 kPa (≈0–1m water depth) |
| MPX5010DP Output | 0.2V (0 kPa) to 4.7V (10 kPa) |
| Formula | `pressure_kPa = (voltage - 0.2) * (10.0 / 4.5)` |
| Formula | `depth_m = pressure_kPa * 0.102` (at sea water: ~0.1) |

**Recommended library:** `pip install adafruit-circuitpython-ads1115`

```python
import busio
import board
import adafruit_ads1115.ads1115 as ADS
from adafruit_ads1115.analog_in import AnalogIn

i2c = busio.I2C(board.SCL, board.SDA)
ads = ADS.ADS1115(i2c, address=0x48)
ads.gain = 1  # ±4.096V
chan = AnalogIn(ads, ADS.P0)  # Single-ended on A0

voltage = chan.voltage
pressure_kPa = (voltage - 0.2) * (10.0 / 4.5)
depth_m = pressure_kPa * 0.102
```

### 3.2 VL53L0X V2 (Obstacle Distance)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x29 |
| Range | 30–2000 mm |
| Resolution | 1 mm |
| Operating Voltage | 2.8–3.3V (use Pi 3.3V rail) |
| Measurement Time | ~30 ms (default) |

**Recommended library:** `pip install adafruit-circuitpython-vl53l0x`

```python
import busio
import board
import adafruit_vl53l0x

i2c = busio.I2C(board.SCL, board.SDA)
sensor = adafruit_vl53l0x.VL53L0X(i2c)
distance_mm = sensor.range  # Returns millimeters
distance_m = distance_mm / 1000.0
```

> **Warning:** VL53L0X is a laser-based sensor. In murky/turbid water, range will be significantly reduced (< 500mm). Works best in clear water.

---

## 4. Camera — OV5647 + COCO-SSD

### 4.1 Camera Setup

Enable camera: `sudo raspi-config` → Interface Options → Camera → Enable

For Pi Zero W: the camera ribbon cable must be the **Zero-specific 15-pin to 22-pin** adapter cable (shorter, different pin pitch).

**Recommended pipeline:**

```python
from picamera2 import Picamera2, Preview
import cv2
import numpy as np
import tflite_runtime.interpreter as tflite

# Initialize camera
picam2 = Picamera2()
config = picam2.create_preview_configuration(
    main={"size": (640, 480), "format": "RGB888"}
)
picam2.configure(config)
picam2.start()

# Load COCO-SSD TFLite model
interpreter = tflite.Interpreter(model_path="coco_ssd_mobilenet.tflite")
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

while True:
    frame = picam2.capture_array()
    # Resize to 300x300 (COCO-SSD input size)
    img = cv2.resize(frame, (300, 300))
    input_data = np.expand_dims(img.astype(np.uint8), axis=0)
    
    interpreter.set_tensor(input_details[0]['index'], input_data)
    interpreter.invoke()
    
    boxes = interpreter.get_tensor(output_details[0]['index'])
    classes = interpreter.get_tensor(output_details[1]['index'])
    scores = interpreter.get_tensor(output_details[2]['index'])
    
    # Filter detections above threshold
    for i in range(len(scores[0])):
        if scores[0][i] > 0.5:
            # Draw bounding box
            pass
```

### 4.2 Object Detection Notes

| Parameter | Value |
|-----------|-------|
| Model | COCO-SSD MobileNet v1 (or v2 for better accuracy) |
| Input size | 300×300 pixels |
| Classes | 90 (person, bottle, fish, etc.) — COCO dataset |
| Inference time | ~200–500ms on Pi Zero W |
| FPS target | 2–5 FPS (limited by Pi Zero CPU) |
| TFLite runtime | `pip install tflite-runtime` (not full TensorFlow) |

**Download model:** https://www.tensorflow.org/lite/models/object_detection/overview

---

## 5. Ballast Control Logic

```
[Surface]                       [Underwater]
   │                                │
   ├─ Expel water (IN3=1, IN4=0)    ├─ Intake water (IN3=0, IN4=1)
   │   → lighter → ascend           │   → heavier → descend
   │                                │
   └─ Hold (IN3=0, IN4=0, ENB=0)   └─ Hold (IN3=0, IN4=0, ENB=0)
```

**Control strategy:**
- Simple: on/off. Run N20 for N seconds to adjust buoyancy
- PID: target depth input → PI controller → N20 PWM
- Safety: define a max ballast run time (e.g., 5 seconds) to prevent running end-stop

**Calibration:** The N20 moves the syringe plunger. At 100% PWM (12V), a typical N20 at 100 RPM moves ~6mm per second. A 20ml syringe has ~60mm travel → ~10 seconds from empty to full.

---

## 6. Control Loop Pseudocode

```
Initialize:
  ├─ GPIO.setmode(GPIO.BCM)
  ├─ Setup all motor pins as OUTPUT
  ├─ Initialize motor PWMs (1000 Hz, 0% duty)
  ├─ Initialize I2C bus
  ├─ Initialize VL53L0X
  ├─ Initialize ADS1115
  ├─ Start camera (picamera2)
  ├─ Load TFLite model
  └─ Start Flask web server

Main Loop (every 100ms):
  ├─ Read sensors (I2C — non-blocking)
  │   ├─ ADS1115 → voltage → pressure → depth
  │   └─ VL53L0X → distance (mm)
  ├─ Read commands (from Web UI / tether)
  │   ├─ Joystick X → side motor PWM & direction
  │   ├─ Joystick Y → rear motor PWM & direction
  │   ├─ Ballast button → N20 on/off
  │   └─ LED toggle → GPIO for LED relay
  ├─ Apply motor outputs
  │   ├─ Set IN1/IN2/ENA for rear motor
  │   ├─ Set IN3/IN4/ENB for side motor
  │   └─ Set IN3/IN4/ENB for ballast (L298N #1 Ch B)
  ├─ Capture frame (every 500ms for detection)
  │   └─ Run TFLite inference on frame
  │       └─ Draw bounding boxes
  ├─ Stream video to web UI (MJPEG)
  └─ Sleep for remaining loop time

Safe Shutdown:
  ├─ Stop all motors (set all IN pins LOW, PWM 0)
  ├─ Release GPIO resources
  ├─ Stop camera
  └─ Clean up
```

---

## 7. Libraries & Installation

```bash
# System dependencies
sudo apt update
sudo apt install -y python3-pip python3-picamera2 python3-opencv \
                    i2c-tools pigpio python3-pigpio

# Python packages
pip install flask flask-socketio tflite-runtime \
            adafruit-circuitpython-ads1115 \
            adafruit-circuitpython-vl53l0x \
            numpy RPi.GPIO

# Enable interfaces
sudo raspi-config  # Enable I2C, Camera, SPI (if needed)
```

---

## 8. Project File Structure

```
SubMarine/
├── ProjectPlan.md
├── WiringPlan.md
├── PowerManagement.md
├── PriceComponents.md
├── src/
│   ├── pins.h              # GPIO definitions (C header)
│   ├── pins.py             # GPIO definitions (Python)
│   ├── CODING_REFERENCE.md # ← This file
│   ├── main.py             # Main control loop
│   ├── motor.py            # Motor driver class
│   ├── sensors.py          # Sensor reader class
│   ├── camera.py           # Camera + TFLite pipeline
│   ├── web_ui.py           # Flask web server + video stream
│   └── ballast.py          # Ballast control class
```

---

## 9. Important Notes

| Concern | Mitigation |
|---------|-----------|
| **GPIO 5 & 6** are also used for SPI0 chip selects | Disable SPI in `raspi-config` if conflict arises |
| **GPIO 2 & 3** have built-in 1.8kΩ pull-up resistors | External pull-ups NOT needed on I2C bus |
| **Pi Zero W WiFi** may interfere with 2.4GHz tether | Use USB-Ethernet tether for reliable control; WiFi for monitoring only |
| **Camera CSI** on Pi Zero requires Zero-specific cable | Standard Pi camera cable is too wide; use adapter |
| **L298N 5V logic** powered from Pi 5V rail | Keep L298N jumpers ON to enable onboard 5V regulator OR feed Pi 5V to L298N +5V pin (do NOT enable both simultaneously) |
| **N20 end-stop** | Syringe has physical limits. Use limit switches or current sensing, or limit run time + track position in software |
| **Battery voltage monitoring** | Route battery voltage through a voltage divider (2× 10kΩ) to ADS1115 A1 for low-battery warning in the web UI |
