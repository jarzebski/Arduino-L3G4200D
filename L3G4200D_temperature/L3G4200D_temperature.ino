/*
    L3G4200D Gyroscope
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-l3g4200d.html

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

  while(!gyroscope.begin())
  {
    Serial.println("Could not find a valid L3G4200D sensor, check wiring!");
    delay(500);
  }
}

void loop()
{
  // Read temperature
  int temp = gyroscope.readTemperature();

  // Output
  Serial.print(" Temp = ");
  Serial.print(temp2);
  Serial.println();

  delay(500);
}
