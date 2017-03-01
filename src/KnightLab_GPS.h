#ifndef __KNIGHTLAB_GPS_H
#define __KNIGHTLAB_GPS_H

#define DEBUG

#include <Adafruit_GPS.h>
#define GPSSerial Serial1
extern Adafruit_GPS GPS;
void setupGPS();

#endif
