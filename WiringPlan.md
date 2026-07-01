# SubMarine ROV — Wiring Plan

Wiring architecture, I²C bus, GPIO pin mapping, and power budget for the SubMarine ROV.

---

## Wiring Architecture

```
                     +--------------------------+
                     ¦   3× 18650 (3S 11.1V)     ¦
                     ¦       2200mAh             ¦
                     +--------------------------+
                              ¦ 11.1V
                    +--------------------+
                    ¦  Voltage Regulator  ¦
                    ¦    12V?5V 3A       ¦
                    +--------------------+
                              ¦ 5V
                    +------------------------+
                    ¦   Raspberry Pi Zero W  ¦
                    ¦        V1.1            ¦
                    +------------------------+
                       ¦   ¦   ¦   ¦   ¦
              +--------+   ¦   ¦   ¦   +----------+
              ¦            ¦   ¦   ¦              ¦
        +-----------+     ¦   ¦   ¦     +-----------------+
        ¦ BTS7960   ¦     ¦   ¦   ¦     ¦  USB?Ethernet   ¦
        ¦ (Rear)    ¦     ¦   ¦   ¦     ¦  (RNDIS tether) ¦
        ¦ GPIO      ¦     ¦   ¦   ¦     +------------------+
        +-----------+     ¦   ¦   ¦              ¦
           ¦   ¦          ¦   ¦   ¦            Tether
      +----+   +----+    ¦   ¦   ¦
   +-----+      +------+ ¦   ¦   +------------------+
   ¦Mtr1 ¦      ¦ Mtr2 ¦ ¦   ¦                      ¦
   ¦Rear ¦      ¦ Side ¦ ¦   ¦                 +-----------+
   ¦Fwd/R¦      ¦ L/R  ¦ ¦   ¦                 ¦ LED Strip ¦
   +-----+      +------+ ¦   ¦                 ¦ (12V)     ¦
                          ¦   ¦                 +-----------+
                    +----------------+
                    ¦   I2C Bus      ¦
                    ¦  SDA -- GPIO2  ¦
                    ¦  SCL -- GPIO3  ¦
                    +----------------+
                       ¦      ¦
              +---------+  +-----------+
              ¦ ADS1115 ¦  ¦ VL53L0X   ¦
              ¦ +MPX5010¦  ¦ V2 (ToF)  ¦
              +---------+  +-----------+

                    +-----------------------+
                    ¦ N20 Motor + Syringe   ¦
                    ¦ (Ballast Control)     ¦
                    ¦ GPIO (via motor drv)  ¦
                    +-----------------------+
```

---

## I2C Bus
| Sensor | Address (I²C) | Pi Pin |
|--------|--------------|--------|
| ADS1115 (MPX5010DP) | 0x48 | GPIO2 (SDA), GPIO3 (SCL) |
| VL53L0X V2 | 0x29 | GPIO2 (SDA), GPIO3 (SCL) |

---

## Pin Mapping (RPi Zero GPIO)
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

---

## Power Budget
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

> **Note:** 3× 18650 (2200mAh each) ˜ 66Wh. At 9A draw ˜ 7–8 min runtime at full load. Consider upgrading to higher capacity or adding more cells in parallel.

---

*This wiring plan is a living document — update as connections change.*
