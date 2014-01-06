/*
    L3G4200D Gyroscope Processing
    Version 1.0
    Read more: http://www.jarzebski.pl/arduino/czujniki-i-sensory/3-osiowy-zyroskop-l3g4200d.html

    Web: http://www.jarzebski.pl
    (c) 2014 by Korneliusz Jarzebski
*/

import processing.serial.*;

Serial myPort;

boolean hasData = false;

int actualSample = 0;
int maxSamples = 600;
int sampleStep = 1;

float[] rxValues = new float[maxSamples+1];
float[] ryValues = new float[maxSamples+1];
float[] rzValues = new float[maxSamples+1];

void setup ()
{
  size(670, 380);
  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.bufferUntil(10);
  background(0);
}

void drawChart(String title, float[] data1, float[] data2, float[] data3, int x, int y, int h) 
{
  strokeWeight(1);
  noFill();
  stroke(50,50,50);
  rect(x, y, (maxSamples*sampleStep)+50, h+50);

  int max = 10;

  strokeWeight(1);
  stroke(90,90,90);

  for (float t = -max; t <= max; t=t+5)
  {
     float line = map(t, -max, max, 0, h);
     line(x+40, y+h-line+16, x+(maxSamples*sampleStep)+40, y+h-line+16);
     fill(200, 200, 200);
     textSize(12);
     text(int(t), 5+x, h-line+20+y);
  }

  String Description;

  Description = "X = "+nf(data1[actualSample-1], 0, 2)+" Y = "+nf(data2[actualSample-1], 0, 2)+" Z = "+nf(data3 [actualSample-1], 0, 2);
  text(Description, ((maxSamples*sampleStep)/2)-(textWidth(Description)/2)+40, h+40+y);

  strokeWeight(1);

  for (int i = 1; i < actualSample; i++)
  {
    stroke(255, 0, 0);
    float v0 = map(data1[i-1], -max, max, 0, h);
    float v1 = map(data1[i],   -max, max, 0, h);
    line(((i-1)*sampleStep)+40+x, h-v0+16+y, (i*sampleStep)+40+x, h-v1+16+y);  

    stroke(0, 255, 0);
    float v2 = map(data2[i-1], -max, max, 0, h);
    float v3 = map(data2[i],   -max, max, 0, h);
    line(((i-1)*sampleStep)+40+x, h-v2+16+y, (i*sampleStep)+40+x, h-v3+16+y);  

    stroke(0, 25, 255);
    float v4 = map(data3[i-1], -max, max, 0, h);
    float v5 = map(data3[i],   -max, max, 0, h);
    line(((i-1)*sampleStep)+40+x, h-v4+16+y, (i*sampleStep)+40+x, h-v5+16+y);  
  }
}

void draw () 
{
  if (!hasData) return;
  background(0);
  drawChart("RAW", rxValues, ryValues, rzValues,  10, 10, 300); 
}
 
void nextSample()
{
  if (actualSample == maxSamples)
  {
    float rxLast = rxValues[maxSamples];
    float ryLast = ryValues[maxSamples];
    float rzLast = rzValues[maxSamples];

    for (int i = 1; i <= (maxSamples-1); i++)
    {
      rxValues[i-1] = rxValues[i];
      ryValues[i-1] = ryValues[i];
      rzValues[i-1] = rzValues[i];
    }

    rxValues[(maxSamples-1)] = rxLast;
    ryValues[(maxSamples-1)] = ryLast;
    rzValues[(maxSamples-1)] = rzLast;
  } else
  {
    actualSample++;
  }
}

void serialEvent (Serial myPort)
{
  String inString = myPort.readStringUntil(10);

  if (inString != null)
  {
    inString = trim(inString);
    String[] list = split(inString, ':');

    String testString = trim(list[0]);

    if (list.length != 3) return;

    float rx = float(list[0]);
    float ry = float(list[1]);
    float rz = float(list[2]);

    rx = rx / 57.2957795;
    ry = ry / 57.2957795;
    rz = rz / 57.2957795;

    if (actualSample == 0)
    {
      for (int i = 0; i <= maxSamples; i++)
      {
        rxValues[i] = rx;
        ryValues[i] = ry;
        rzValues[i] = rz;
      }
    }

    rxValues[actualSample] = rx;
    ryValues[actualSample] = ry;
    rzValues[actualSample] = rz;

    if (actualSample > 1)
    {
      hasData = true;
    }

    nextSample();
  }
}
