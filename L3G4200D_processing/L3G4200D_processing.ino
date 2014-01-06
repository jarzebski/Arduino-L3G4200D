/*
    L3G4200D example for processing
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-l3g4200d.html

    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <L3G4200D.h>

L3G4200D gyroscope;

int LED = 13;
boolean Blink = false;

void setup() 
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  // Initialize L3G4200D
  // 250 dps: L3G4200D_250DPS
  // 500 dps: L3G4200D_500DPS
  // 2000 dps: L3G4200D_2000DPS
  while (!gyroscope.begin(L3G4200D_2000DPS))
  {
    // Waiting for initialization

    if (Blink)
    {
      digitalWrite(LED, HIGH);
    } else
    {
      digitalWrite(LED, LOW);
    }

    Blink = !Blink;

    delay(500);
  }

  digitalWrite(LED, HIGH);

  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  gyroscope.calibrate();

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  gyroscope.setThreshold(3);

  digitalWrite(LED, LOW);
  Blink = false;
}

void loop() 
{
  // Read normalized values 
  Vector norm = gyroscope.readNormalize();

  // Output
  Serial.print(norm.XAxis);
  Serial.print(":");
  Serial.print(norm.YAxis);
  Serial.print(":");
  Serial.print(norm.ZAxis);
  Serial.println();

  // Output indicator
  if (Blink)
  {
    digitalWrite(LED, HIGH);
  } else
  {
    digitalWrite(LED, LOW);
  }

  Blink = !Blink;
}
