# Changelog

## v1.5.2-patch.1

- Backport : fix a regression
  (_the PPeak+ button was not functioning anymore_)

## v1.5.2

- improve pressure control

## v1.5.1

- calibrate pressure sensor's offset on startup
- better round displayed pressure values (in cmH2O)
- minor improvement to the pressure control

## v1.5.0

- rework pressure and blower control
  (_blower will now take more time to ramp up/down but this will greatly improve the stability of injected air volume in many scenario_)
- rework alarms
  (_better pressure alarms, better battery alarms, better snooze behavior_)
- improve even more the measured and displayed pressures
  (_if no plateau pressure if found, screen will now display a `?` instead of an uncertain value_)
- tweak the pressure control to make the plateau more accurate
- fix an issue with blower not restarting in some cases
- make sysclock more accurate
- improve code quality (MISRA)
- add a step in integration test to check O2 pipe

## v1.4.0

_This release was depublished_

## v1.3.2

- fix blower control
  (_it used to unexpectedly slow down a bit from times to times_)
- improve the measured and displayed pressures for peak and plateau

## v1.3.1

- use the green LED near the start button to show whether the breathing mode is ON or not
- disable alarms related to the breathing cycle when program is stopped
- fix an issue with a battery alarm being briefly triggered at every boot
- integration test: open both valves at startup

## v1.3.0

- add a program to test integration
- support electronic hardware v1 **and** v2
  (_v1 by default, v2 through a config flag_)
- implement start/stop
  (_from now on, machine will begin stopped!_)
- handle Emerson valves
  (_through a config flag_)
- improve the way buzzer is controlled

## v1.2.3

- fix an issue with buzzer (sometimes it was stuck buzzing whereas no alarm were triggered)
- add battery-related alarms

## v1.2.2

- handle several pneumatic systems
  _(for the first 2 working typologies of prototype)_

## v1.2.1

_Unreleased_

## v1.2.0

First release version
