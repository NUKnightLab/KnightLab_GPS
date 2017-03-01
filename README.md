# KnightLab_GPS

A library for simplifying usage of the Adafruit GPS library on Feather 32u4 &
M0 boards.

Sets up timer-based interrupts to read GPS data. Exposes `setupGPS` which you
must call during setup, and `GPS` which is an instance of Adafruit_GPS (see:
https://github.com/adafruit/Adafruit_GPS/blob/master/Adafruit_GPS.h)


## To use:
  - include `<KnightLab_GPS.h>`
  - call `setupGPS()` in your setup
  - read values from GPS (see `Adafruit_GPS` class for details)

## Notes:
  - define DEBUG for small amount of Serial output
  - non-interrupt option for 32u4 is not really being used here
  - period register value of 1111 was an arbitrary choice that seems to be working for now


This library has been extracted from the Knight Lab Sensor Grid project. Design is
pretty specific to the needs of that project, but has no Sensor Grid dependencies.
If you find it useful, let us know.
