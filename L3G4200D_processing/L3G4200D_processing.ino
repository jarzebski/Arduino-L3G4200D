/*
    L3G4200D example for processing
    Version 1.0
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

  Wire.begin();

  // Initialize L3G4200D
  // 250 dps: L3G4200D_250DPS
  // 500 dps: L3G4200D_500DPS
  // 2000 dps: L3G4200D_2000DPS
  if (gyroscope.begin(L3G4200D_2000DPS))
  {
    // Calibrate gyroscope. The calibration must be at rest.
    // If you don't want calibrate, comment this line.
    gyroscope.calibrate();

    // Set threshold sensivty. Default 3.
    // If you don't want use threshold, comment this line or set 0.
    gyroscope.setThreshold(3);
  }
}

void loop() 
{
  // Read normalized values 
  GyroscopeNormalize  norm = gyroscope.readNormalize();

  // Output
  Serial.print(norm.XAxis);
  Serial.print(":");
  Serial.print(norm.YAxis);
  Serial.print(":");
  Serial.print(norm.ZAxis);
  Serial.println();
}
