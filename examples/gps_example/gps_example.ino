#include <KnightLab_GPS.h>


void displayGPS()
{
    /* other properties are available. See:
     * https://github.com/adafruit/Adafruit_GPS/blob/master/Adafruit_GPS.cpp
     */
    Serial.println(GPS.lastNMEA());
    Serial.print("DT: ");
    Serial.print(GPS.month, DEC); Serial.print("/");
    Serial.print(GPS.day, DEC); Serial.print("/");
    Serial.print(GPS.year, DEC); Serial.print(" ");
    Serial.print(GPS.hour, DEC); Serial.print(":");
    Serial.print(GPS.minute, DEC); Serial.print(":");
    Serial.println(GPS.seconds, DEC);
    Serial.print("GPS fix: "); Serial.print(GPS.fix);
    Serial.print("; LAT: "); Serial.print(GPS.latitudeDegrees);
    Serial.print("; LON: "); Serial.println(GPS.longitudeDegrees);
    Serial.println("---");
}

void setup()
{
    setupGPS();
}

void loop()
{
    // display info every 10 sec
    displayGPS();
    delay(10 * 1000);
}
