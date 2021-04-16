# TODOs

## Features
- Restructure menus
- Timers
    - Add Pomodoro/chaining mode (set follow up timers: -, 1, 2, etc)
    - Add pause to timers (long press enter)
- Add pulsar mode (blinky blinky)
    - BPM capture by button press
- Alarms
    - Add light as alarm signal
- Breathing app
- Remove OTP

## HAL
- Add support for the other two LEDs (PJ.7, P7.2/T0.1) ✓
- Repair the PWM bug (P1.0/TA0.1 is PWM-capable, not PJ.1)
- Execute LED service routine in 16 Hz button polling interrupt instead of AUX timer

## Simulator
- Add the other two LEDs to the simulator ✓
- Make key bindings to streamline debugging

## Desired menu structure

### Main apps:
- time
- countdowns
- alarms
- stopwatch
- pulsar
- breathing
