# TODOs

## Features
- Time
    - Seconds should not be settable. Instead clicking on seconds should reset them to 0 like in the original Casio
- Restructure menus
    - Move Play app to config menu
    - Add config menu by long up press
    - Indicate which menu we are in with blinking top header
    - Long enter press in main view of any app should always redirect to time
- Timers
    - There is only one pending timer at a time
- Add pulsar mode (blinky blinky)
    - BPM capture by button press
- Alarms
    - Add light as alarm signal
- Breathing app
- Remove OTP

### Pulsar app
- Features
    - BPM capture ✓
    - First beat capture ✓
    - Make use of all LEDs ✓
    - BPM display ✓
    - Different flashing modes ✓
    - Offer metronome click
- Default view
    - Show BPM ✓
    - Flash display segment with beat (or other animation)
    - up button captures BPM ✓
- Main menu
    - Animation modes (choice menu)
    - BPM set (overrides capture)

## HAL
- Add support for the other two LEDs (PJ.7, P7.2/T0.1) ✓
- Repair the PWM bug (P1.0/TA0.1 is PWM-capable, not PJ.1)
- Execute LED service routine in 16 Hz button polling interrupt instead of AUX timer
- Or (!!!) put all LED handling to AUX timer and soft-PWM them. Three brightness levels @ 128 HZ -> 32 HZ.
- Add hal_click to target and sim

## Desired menu structure

### Main apps:
- time
- countdowns
- alarms
- stopwatch
- pulsar
- breathing

## Bugs
- There could be problems with masked blinking on digits 8 and 9
