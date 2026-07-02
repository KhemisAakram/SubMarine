// =============================================
// SubMarine ROV — GPIO Pin Definitions
// Raspberry Pi Zero W V1.1 (BCM numbering)
// =============================================

#ifndef PINS_H
#define PINS_H

// --- L298N #1 — Rear Motor (Channel A) + Ballast (Channel B) ---

// Channel A — Rear 34900 Motor (Forward / Reverse)
#define REAR_IN1    17   // BCM17  / Pin 11  — Dir A
#define REAR_IN2    18   // BCM18  / Pin 12  — Dir B
#define REAR_ENA    27   // BCM27  / Pin 13  — PWM speed

// Channel B — N20 Ballast Motor (Syringe push/pull)
#define BALL_IN3     5   // BCM5   / Pin 29  — Dir A
#define BALL_IN4     6   // BCM6   / Pin 31  — Dir B
#define BALL_ENB    19   // BCM19  / Pin 35  — PWM speed

// --- L298N #2 — Side Motor (Left / Right) ---
#define SIDE_IN3    22   // BCM22  / Pin 15  — Dir A
#define SIDE_IN4    23   // BCM23  / Pin 16  — Dir B
#define SIDE_ENB    24   // BCM24  / Pin 18  — PWM speed

// --- I2C Bus ---
#define I2C_SDA      2   // BCM2   / Pin 3   — Data
#define I2C_SCL      3   // BCM3   / Pin 5   — Clock

// I2C Device Addresses
#define ADDR_ADS1115  0x48   // ADS1115 + MPX5010DP (pressure)
#define ADDR_VL53L0X  0x29   // VL53L0X V2 (ToF distance)

// Camera — OV5647 via CSI ribbon (no GPIO)

// Power — 5V from LM2596 buck converter
#define VREG_5V       5   // Physical Pin 2  — 5V input to Pi
#define GND_PIN       6   // Physical Pin 9  — Ground

// --- Motor Logic ---
// L298N operation (example: REAR motor):
//   IN1=0  IN2=0  → Brake (stop)
//   IN1=1  IN2=0  → Forward
//   IN1=0  IN2=1  → Reverse
//   IN1=1  IN2=1  → Brake
//
// Ballast (via L298N #1 Ch B):
//   IN3=1  IN4=0  ENB=PWM  → Expel water → lighter → ascend
//   IN3=0  IN4=1  ENB=PWM  → Intake water → heavier → descend
//   IN3=0  IN4=0  ENB=0    → Hold position

#endif // PINS_H
