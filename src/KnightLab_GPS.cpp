/**
 * KnightLab_GPS.cpp
 *
 * Written by Scott B. Bradley (@scott2b) for Knight Lab
 *
 * Mostly adapted from Adafruit GPS examples. See:
 * https://github.com/adafruit/Adafruit_GPS/blob/master/Adafruit_GPS.h
 *
 * M0 TC0 interrupt code adapted from examples by Markus Bader. See:
 *
 * https://github.com/maxbader/arduino_tools/blob/master/libraries/timer_zero_tcc_interrupt/timer_zero_tcc_interrupt.ino
 *
 * To use:
 *      - include <KnightLab_GPS.h>
 *      - call setupGPS() in your setup
 *      - read values from GPS (see Adafruit_GPS class for details)
 *
 * Notes:
 *      - Current support for AVR and SAMD, tested on Feather 32u4 and M0 only
 *      - define DEBUG for small amount of Serial output
 *      - non-interrupt option for 32u4 is not really being used here
 *      - period register value of 1111 was an arbitrary choice that seems to be working for now
 */
#include "KnightLab_GPS.h"

#ifdef DEBUG
    #define DEBUG_PRINT(x)  Serial.print(x)
#else
    #define DEBUG_PRINT(x)
#endif

Adafruit_GPS GPS(&GPSSerial);

static void _readGPS() {
    // Should be called from a timer interrupt. Only reads 1 char at a time!
    char c = GPS.read();
    if (GPS.newNMEAreceived()) {
        // a tricky thing here is if we print the NMEA sentence, or data
        // we end up not listening and catching other sentences!
        // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
        if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
            return; // we can fail to parse a sentence in which case we should just wait for another
    }
}

#if defined(__AVR_ATmega32U4__)

boolean usingInterrupt_32u4 = true;

static void _useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt_32u4 = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt_32u4 = false;
  }
}

SIGNAL(TIMER0_COMPA_vect) {
    _readGPS();
}

static void _setupGPSInterrupt() {
    _useInterrupt(usingInterrupt_32u4);
}
#elif defined(ARDUINO_ARCH_SAMD)

/**
 * Note: Generic Clock Multiplexer IDs for M0/SAMD are defined here:
 * https://github.com/arduino/ArduinoCore-samd/blob/13b371ba371aefcd2da9e71b4694d93641eeeaae/cores/arduino/WVariant.h
 */

static void _setupGPSInterrupt() {
    REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC0_TCC1) ;
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync
    // The type cast must fit with the selected timer
    Tcc* TC = (Tcc*) TCC0; // get timer struct
    TC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;   // Disable TC
    while (TC->SYNCBUSY.bit.ENABLE == 1); // wait for sync
    TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV256;   // Set perscaler
    TC->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ;   // Set wave form configuration
    while (TC->SYNCBUSY.bit.WAVE == 1); // wait for sync
    //TC->PER.reg = 0xFFFF;              // Set counter Top using the PER register
    TC->PER.reg = 0x1111; // This may need some tweaking for a good GPS read
    while (TC->SYNCBUSY.bit.PER == 1); // wait for sync
    TC->CC[0].reg = 0xFFF;
    while (TC->SYNCBUSY.bit.CC0 == 1); // wait for sync
    TC->INTENSET.reg = 0;                 // disable all interrupts
    TC->INTENSET.bit.OVF = 1;          // enable overfollow
    TC->INTENSET.bit.MC0 = 1;          // enable compare match to CC0
    NVIC_EnableIRQ(TCC0_IRQn);
    TC->CTRLA.reg |= TCC_CTRLA_ENABLE ;
    while (TC->SYNCBUSY.bit.ENABLE == 1); // wait for sync
}

void TCC0_Handler(){
    Tcc* TC = (Tcc*) TCC0;       // get timer struct
    if (TC->INTFLAG.bit.OVF == 1) {  // A overflow caused the interrupt
        TC->INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
    }
    if (TC->INTFLAG.bit.MC0 == 1) {  // A compare to cc0 caused the interrupt
        TC->INTFLAG.bit.MC0 = 1;    // writing a one clears the flag ovf flag
        _readGPS();
    }
}
#else
    static void _setupGPSInterrupt();
    #error "Unsupported architecture for use with KnightLab_GPS library. Support currently provided for AVR32u4 and SAMD chipsets. Known working boards are Adafruit Feather 32u4 & M0"
#endif

void setupGPS() {
    DEBUG_PRINT("INIT GPS..");
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    _setupGPSInterrupt();
    DEBUG_PRINT("  ..GPS INIT\n"); // TODO: is there a way to tell GPS init is unsuccessful?
    delay(500);
}
