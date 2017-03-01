#include <KnightLab_GPS.h>


void displayGPS()
{
    /* other properties are available. See:
     * https://github.com/adafruit/Adafruit_GPS/blob/master/Adafruit_GPS.cpp
     *  TODO: better formatting
     */
    Serial.print("DT: 20");
    Serial.print(GPS.year); Serial.print("-");
    Serial.print(GPS.month); Serial.print("-");
    Serial.print(GPS.day); Serial.print("T");
    Serial.print(GPS.hour); Serial.print(":");
    Serial.print(GPS.minute); Serial.print(":");
    Serial.println(GPS.seconds);
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
