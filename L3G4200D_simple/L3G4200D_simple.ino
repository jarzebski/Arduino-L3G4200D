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

  Wire.begin();

  // Initialize L3G4200D
  // 250 dps: L3G4200D_250DPS
  // 500 dps: L3G4200D_500DPS
  // 2000 dps: L3G4200D_2000DPS
  Serial.println("Initialize L3G4200D");

  while(!gyroscope.begin(L3G4200D_2000DPS))
  {
    Serial.println("Could not find a valid L3G4200D sensor, check wiring!");
    delay(500);
  }

  // Check selected scale
  Serial.print("Selected scale: ");

  switch(gyroscope.getScale())
  {
    case L3G4200D_250DPS:
      Serial.println ("250 dps");
      break;
    case L3G4200D_500DPS:
      Serial.println ("500 dps");
      break;
    case L3G4200D_2000DPS:
      Serial.println ("2000 dps");
      break;
  }

  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  gyroscope.calibrate();

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  gyroscope.setThreshold(3);
}

void loop()
{
  // Read normalized values
  Vector raw = gyroscope.readRaw();

  // Read normalized values
  Vector norm = gyroscope.readNormalize();

  // Output raw
  Serial.print(" Xraw = ");
  Serial.print(raw.XAxis);
  Serial.print(" Yraw = ");
  Serial.print(raw.XAxis);
  Serial.print(" Zraw = ");
  Serial.print(raw.YAxis);

  // Output normalized
  Serial.print(" Xnorm = ");
  Serial.print(norm.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(norm.YAxis);
  Serial.print(" ZNorm = ");
  Serial.print(norm.ZAxis);

  Serial.println();
}
