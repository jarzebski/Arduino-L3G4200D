/*
    L3G4200D Triple Axis Gyroscope. Read temperature.
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-l3g4200d.html
    GIT: https://github.com/jarzebski/Arduino-L3G4200D
    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <L3G4200D.h>

L3G4200D gyroscope;

void setup() 
{
  Serial.begin(9600);

  Serial.println("Initialize L3G4200D");

  // Initialize L3G4200D
  while(!gyroscope.begin())
  {
    Serial.println("Could not find a valid L3G4200D sensor, check wiring!");
    delay(500);
  }
}

void loop()
{
  // Read temperature.
  // L3G4200D Temperature sensor output change vs temperature: -1digit/degrCelsius (data representation: 2's complement).
  // Value represents difference respect to a reference not specified value.
  // So temperature sensor can be used to measure temperature variations: temperarture sensor isn't suitable to return absolute temperatures measures.
  // If you run two sequential measures and differentiate them you can get temperature variation.
  // This also means that two devices in the same temp conditions can return different outputs.
  // Finally, you can use this info to compensate drifts due to temperature changes.
  int temp = gyroscope.readTemperature();

  // Output
  Serial.print(" Temp = ");
  Serial.print(temp);
  Serial.println();

  delay(500);
}
