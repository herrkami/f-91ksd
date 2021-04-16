# TODOs

## Features
- Remove compass ✓
- Remove speed ✓
- Remove IR stuff (if there??) ✓
- Remove OTP
- Restructure menus
- Timers
    - Add Pomodoro/chaining mode (set follow up timers: -, 1, 2, etc)
    - Add pause to timers
    - Timers shouldn't reset after reaching 0 (bug)
- Breathing app
- Add club mode (blinky blinky)
    - BPM capture by button press
- Alarms
    - Only one alarm exists so far (bug)

## HAL
- Add support for the other two LEDs (PJ.7, P7.2/T0.1) ✓
- Repair the PWM bug (P1.0/TA0.1 is PWM-capable, not PJ.1)
- Execute LED service routine in 16 Hz button polling interrupt instead of AUX timer

## Simulator
- Add the other two LEDs to the simulator ✓
- Make key bindings to streamline debugging
