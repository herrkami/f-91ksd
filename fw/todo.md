# TODOs

## Todo next
- Add metronome clicks for pulsar mode
- Fix calibration routine
- Better and more LED sequences
    - Make sequences ordered by signature
- Add breathing app
- Better and more melodies
- hour beep_hour_quiet defines that hour beep should be off during night time


- BUG: Switching time zone affects seconds ✓
- Fix second adjustment ✓
- Limit brightness to only four stages ✓
- Countdowns should be derived from RTC actually (svc_countdown_dec())?
- Add flashlight app for case led ✓
    - Eat event if flashlight is disabled by click ✓
- Running counters should be visible and easy to access ✓
    - The |||||-symbol should blink while a counter is running ✓
    - A popup window should show the current counter status ✓
    - Make the routine more efficient! I.e., prevent time from being redrawn every time when counters are running. Introduce a mode like `display_date` instead. ✓

- Fix the melody sound bug: releasing pending timers should also stop sounds ✓
- Get default BPM from pulsar sequences for alarms ✓
- Preset alarms with historic points in time ✓


### Pulsar app
- Read from RTC!
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
    - Animation modes (choice menu) ✓
    - BPM set (overrides capture) ✓

## HAL
- Add support for the other two LEDs (PJ.7, P7.2/T0.1) ✓
- Repair the PWM bug (P1.0/TA0.1 is PWM-capable, not PJ.1)
- Execute LED service routine in 16 Hz button polling interrupt instead of AUX timer
- Or (!!!) put all LED handling to AUX timer and soft-PWM them. Three brightness levels @ 128 HZ -> 32 HZ. ✓
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
- Switching time zone affects seconds
- The first pulsar measurement interval is usually around 100 ms shorter than the succeeding intervals. ✓
- There could be problems with masked blinking on digits 8 and 9 ✓
- aux timer routine sometimes dropped! e.g. svc_pulsar_measure_tap_handler() doesn't get called ✓
