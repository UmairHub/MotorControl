# STM32 Nucleo-F446RE + L298N DC Motor Control — v1

Wiring reference and pinout for driving a single DC motor via PWM speed control and H-bridge direction control, as part of the v1 autonomous robot build (bare-metal register-level firmware).

## Pin Connections

| L298N Pin | Nucleo Pin | Function                          |
|-----------|------------|------------------------------------|
| ENA       | PA6        | PWM output (TIM3_CH1) — speed control |
| IN1       | PA8        | GPIO output — direction control bit 1 |
| IN2       | PA9        | GPIO output — direction control bit 2 |
| GND       | GND        | Common ground (Nucleo + L298N + battery) |
| 12V / VMS | Battery +  | Motor power input (9V battery)     |
| OUT1      | —          | To DC motor terminal 1             |
| OUT2      | —          | To DC motor terminal 2             |

## Power Wiring

- **9V battery positive** → L298N `12V / VMS` input
- **9V battery negative (GND)** → tied to the same common ground as the Nucleo and L298N
- **Nucleo is powered separately** (e.g. via USB) — never connect the Nucleo's own supply to the motor's power rail
- All three grounds (battery, Nucleo, L298N) **must** share a single common reference point

## Notes

- Remove the L298N's **ENA jumper cap** if present — it forces the driver to always-full-speed and blocks PWM control on that channel.
- **PWM configuration**: TIM3, Channel 1, ~1kHz frequency (PSC and ARR chosen based on actual APB1 timer clock — recalculate if `SystemClock_Config()` changes).
- **Direction truth table**:
  | IN1  | IN2  | Result   |
  |------|------|----------|
  | HIGH | LOW  | Forward  |
  | LOW  | HIGH | Reverse  |
  | LOW  | LOW  | Stop     |
- A 9V battery has limited current capacity — expect possible voltage sag or reduced torque under load. Switch to a higher-capacity pack (e.g. 4–6x AA) if the motor struggles or the Nucleo resets under load.
- Firmware is written bare-metal (direct register access), not using HAL APIs.

## Build order (v1 roadmap)

1. Open-loop motor test (this document) — PWM + direction, no encoders
2. Wire and verify quadrature encoders
3. Closed-loop speed control (PID using encoder feedback)
4. Add IMU (MPU6050) for orientation
5. Add distance sensors (VL53L0X or HC-SR04) for obstacle detection
6. Reactive avoidance logic — v1 complete
