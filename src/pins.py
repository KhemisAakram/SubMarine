# SubMarine ROV — GPIO Pin Definitions
# Raspberry Pi Zero W V1.1 (BCM numbering)

# --- L298N #1 — Rear Motor (Channel A) ---
REAR_IN1 = 17   # GPIO17 / Pin 11 — Dir A
REAR_IN2 = 18   # GPIO18 / Pin 12 — Dir B
REAR_ENA = 27   # GPIO27 / Pin 13 — PWM speed

# --- L298N #1 — Ballast (Channel B, N20 syringe) ---
BALL_IN3 = 5    # GPIO5  / Pin 29 — Dir A
BALL_IN4 = 6    # GPIO6  / Pin 31 — Dir B
BALL_ENB = 19   # GPIO19 / Pin 35 — PWM speed

# --- L298N #2 — Side Motor (Channel B) ---
SIDE_IN3 = 22   # GPIO22 / Pin 15 — Dir A
SIDE_IN4 = 23   # GPIO23 / Pin 16 — Dir B
SIDE_ENB = 24   # GPIO24 / Pin 18 — PWM speed

# --- I2C ---
I2C_SDA = 2
I2C_SCL = 3
ADDR_ADS1115 = 0x48
ADDR_VL53L0X = 0x29
