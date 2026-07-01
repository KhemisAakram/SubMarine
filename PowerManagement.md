# SubMarine ROV — Power Management

Power architecture, voltage regulation, protection, and runtime calculations.

---

## Power Tree

```
                     3× 18650 (3S 11.1V)
                      │
                      ├── 10A Fuse
                      │
                      ├── BMS 3S 12.6V
                      │
                      ├───────────────────── 12V Rail ─────────────────────┐
                      │                       │                            │
                      │                       │                    ┌───────┴────────┐
                      │                       │                    │ LED Strip (12V)│
                      │                       │                    └────────────────┘
                      │                       │
                      │              ┌────────┴────────┐
                      │              │ Buck Converter   │
                      │              │ LM2596 (12V→5V)  │
                      │              │    3A max        │
                      │              └────────┬────────┘
                      │                       │ 5V
                      │                       │
                      │              ┌────────┴──────────────────────┐
                      │              │           5V Rail             │
                      │              │                               │
                      │     ┌────────┴────────┐   ┌──────────────────┴───┐
                      │     │  Pi Zero W 5V   │   │  BTS7960 Logic (x2)   │
                      │     │  (+200mA)       │   │  (+50mA total)         │
                      │     └─────────────────┘   └──────────────────────┘
                      │     ┌─────────────────┐   ┌──────────────────────┐
                      │     │  N20 Motor (5V)  │   │ ADS1115 + MPX5010DP  │
                      │     │  (+100mA)        │   │  (+10mA)              │
                      │     └─────────────────┘   └──────────────────────┘
                      │
                      │  ┌─────────────────────────────────────────────┐
                      └──┤ 12V Rail → BTS7960 Power Input → 2× 34900 │
                         │ Motors (4–8A under load)                    │
                         └─────────────────────────────────────────────┘
```

---

## Battery Pack

| Parameter | Value |
|-----------|-------|
| Configuration | 3S (3× 18650 in series) |
| Cell Spec | 18650 3.7V 2200mAh each |
| Nominal Voltage | 11.1V |
| Full Charge | 12.6V |
| Cut-off | ~9.0V (BMS protected) |
| Energy | 24.4 Wh |
| Connector | XT60 (main power out) |

### BMS Requirements
- 3S 12.6V Li-ion BMS
- Continuous current rating: ≥12A
- Over-discharge protection at ~2.8V/cell (8.4V pack)
- Over-charge protection at 4.25V/cell (12.75V pack)
- Balance function (passive balancing during charge)

---

## Voltage Regulator

| Parameter | Value |
|-----------|-------|
| Type | Buck converter (switching) |
| Model | LM2596-based module (or Pololu D36V28F5) |
| Input | 9.6V – 12.6V (3S range) |
| Output | 5V ±2% |
| Current | 3A max (derated: 2.5A continuous) |
| Efficiency | ~85–92% |

> **Why buck:** Linear regulators (7805, etc.) would waste ~7V × 1A = 7W as heat inside the sealed PVC tube — a buck converter keeps heat under control.

---

## Power Budget

### 12V Rail (direct from battery)
| Component | Current (est.) | Notes |
|-----------|---------------|-------|
| 2× 34900 Motors (idle) | ~200mA | No load |
| 2× 34900 Motors (cruise) | ~2–3A | Light load |
| 2× 34900 Motors (full) | ~4–8A | Max load / stall |
| LED Strip (12V) | ~500mA | Full brightness |
| **12V Total (cruise)** | **~2.5–3.5A** | |
| **12V Total (max)** | **~8.5A** | |

### 5V Rail (via buck converter)
| Component | Current (est.) | Notes |
|-----------|---------------|-------|
| Pi Zero W | ~200mA | Idle + camera |
| BTS7960 Logic (×2) | ~50mA | GPIO PWM control |
| N20 Motor | ~100mA | Syringe actuation |
| ADS1115 + MPX5010DP | ~10mA | I²C sensors |
| **5V Total** | **~360mA** | Well within 3A limit |

### 3.3V Rail (from Pi Zero internal reg)
| Component | Current (est.) | Notes |
|-----------|---------------|-------|
| OV5647 Camera | ~250mA | Via Pi CSI |
| VL53L0X V2 | ~20mA | I²C ToF sensor |
| **3.3V Total** | **~270mA** | |

---

## Runtime Estimation

| Scenario | Total Draw | Theoretical | Real (×0.8 eff) |
|----------|-----------|-------------|-----------------|
| Full throttle + LED | 9A | 2.7 min | **~2 min** |
| Cruise + LED | 4A | 6.1 min | **~5 min** |
| Cruise no LED | 3.5A | 7 min | **~5.5 min** |
| Idle (sensors only) | 0.5A | 48 min | **~38 min** |

> **⚠️ 2200mAh is very limited for this setup.** The 34900 motors draw 2–4A each under load — one full-throttle burst drains the pack in ~2–3 minutes.

### Capacity Upgrade Options
| Option | Capacity | Cruise Runtime |
|--------|----------|---------------|
| Current: 3S 2200mAh | 24.4 Wh | ~5 min |
| 3S 4400mAh (2 packs parallel) | 48.8 Wh | ~10 min |
| 3S 6600mAh (3 packs parallel) | 73.2 Wh | ~15 min |
| 4S 5000mAh LiPo | 74 Wh | ~15 min |
| 6S 5000mAh LiPo + dual reg | 111 Wh | ~22 min |

**Recommended minimum:** 3S 5000mAh LiPo (or 2× 3S 2200mAh in parallel = 4400mAh).

---

## Protection & Safety

| Protection | Implementation |
|------------|---------------|
| Over-current | 10A ATC/ATO blade fuse on main 12V line |
| Over-discharge | 3S BMS (cut-off at ~9V pack) |
| Over-charge | BMS balance charging + charger cutoff |
| Reverse polarity | Schottky diode (or P-MOSFET ideal diode) on input |
| Short circuit | Fuse + BMS both provide protection |
| Thermal | Monitor battery temp (NTC on BMS) |

---

## Wire Gauge

| Rail | Gauge | Type |
|------|-------|------|
| Battery → Fuse → BTS7960 | 16 AWG | Silicone (high-flex) |
| 12V Distribution | 18 AWG | Silicone |
| 5V Distribution | 22 AWG | Standard |
| I²C / Signal | 24–26 AWG | Twisted pair for SDA/SCL |
| Tether (power pair) | 18 AWG ×2 | Shielded multi-core |
| Tether (data pair) | 24 AWG twisted | Shielded |

---

## Tether Power Considerations

For a **10m+ tether**:
- 18 AWG copper has ~6.4 mΩ/m → 10m × 2 (out+back) = 128 mΩ
- At 4A: Vdrop = 0.51V → Pi sees ~10.6V at input → buck converter fine
- At 9A: Vdrop = 1.15V → Pi sees ~10V → still above 9V BMS cutoff
- **Recommendation:** Use 18 AWG or thicker for tether power pair. If tether exceeds 15m, step up to 16 AWG or inject power locally at the sub.

---

## Wiring Notes
- **Ground bus:** Star-connect all grounds at battery negative terminal
- **Decoupling:** Add 100µF + 0.1µF ceramic cap at each BTS7960 power input
- **Pi power:** Feed 5V directly to Pi Zero 5V pin (not via microUSB — more stable)
- **Motor snubbers:** Add flyback diodes across each 34900 motor (already on BTS7960, but verify)
- **BMS wiring:** Use balance lead connector for charging (standard 3S JST-XH)

---

*This power plan is a living document — update as components change.*
