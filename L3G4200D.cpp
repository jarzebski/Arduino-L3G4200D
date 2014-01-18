/*
L3G4200D.cpp - Class file for the L3G4200D Triple Axis Gyroscope Arduino Library.

Version: 1.3.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <math.h>

#include <L3G4200D.h>

boolean L3G4200D::begin(dps_t scale, odrbw_t odrbw)
{
    // Reset calibrate values
    d.XAxis = 0;
    d.YAxis = 0;
    d.ZAxis = 0;
    useCalibrate = false;

    // Reset threshold values
    t.XAxis = 0;
    t.YAxis = 0;
    t.ZAxis = 0;
    actualThreshold = 0;

    Wire.begin();

    // Check L3G4200D Who Am I Register
    if (fastRegister8(L3G4200D_WHO_AM_I) != 0xD3)
    {
	return false;
    }

    // Enable all axis and setup normal mode + Output Data Range & Bandwidth
    uint8_t reg1 = 0x00;
    reg1 |= 0x0F; // Enable all axis and setup normal mode
    reg1 |= (odrbw << 4); // Set output data rate & bandwidh
    writeRegister8(L3G4200D_CTRL_REG1, reg1);

    // Disable high pass filter
    writeRegister8(L3G4200D_CTRL_REG2, 0x00);

    // Generata data ready interrupt on INT2
    writeRegister8(L3G4200D_CTRL_REG3, 0x08);

    // Set full scale selection in continous mode
    writeRegister8(L3G4200D_CTRL_REG4, scale << 4);

    switch(scale)
    {
	case L3G4200D_250DPS:
	    dpsPerDigit = .00875f;
	    break;
	case L3G4200D_500DPS:
	    dpsPerDigit = .0175f;
	    break;
	case L3G4200D_2000DPS:
	    dpsPerDigit = .07f;
	    break;
	default:
	    break;
    }

    // Boot in normal mode, disable FIFO, HPF disabled
    writeRegister8(L3G4200D_CTRL_REG5, 0x00);

    return true;
}

// Get current scale
dps_t L3G4200D::getScale(void)
{
    return (dps_t)((readRegister8(L3G4200D_CTRL_REG4) >> 4) & 0x03);
}


// Get current output data range and bandwidth
odrbw_t L3G4200D::getOdrBw(void)
{
    return (odrbw_t)((readRegister8(L3G4200D_CTRL_REG1) >> 4) & 0x0F);
}

// Calibrate algorithm
void L3G4200D::calibrate(uint8_t samples)
{
    // Set calibrate
    useCalibrate = true;

    // Reset values
    float sumX = 0;
    float sumY = 0;
    float sumZ = 0;
    float sigmaX = 0;
    float sigmaY = 0;
    float sigmaZ = 0;

    // Read n-samples
    for (uint8_t i = 0; i < samples; ++i)
    {
	readRaw();
	sumX += r.XAxis;
	sumY += r.YAxis;
	sumZ += r.ZAxis;

	sigmaX += r.XAxis * r.XAxis;
	sigmaY += r.YAxis * r.YAxis;
	sigmaZ += r.ZAxis * r.ZAxis;
	
	delay(5);
    }

    // Calculate delta vectors
    d.XAxis = sumX / samples;
    d.YAxis = sumY / samples;
    d.ZAxis = sumZ / samples;

    // Calculate threshold vectors
    thresholdX = sqrt((sigmaX / 50) - (d.XAxis * d.XAxis));
    thresholdY = sqrt((sigmaY / 50) - (d.YAxis * d.YAxis));
    thresholdZ = sqrt((sigmaZ / 50) - (d.ZAxis * d.ZAxis));

    // If already set threshold, recalculate threshold vectors
    if (actualThreshold > 0)
    {
	setThreshold(actualThreshold);
    }
}

// Get current threshold value
uint8_t L3G4200D::getThreshold(void)
{
    return actualThreshold;
}

// Set treshold value
void L3G4200D::setThreshold(uint8_t multiple)
{
    if (multiple > 0)
    {
	// If not calibrated, need calibrate
	if (!useCalibrate)
	{
	    calibrate();
	}
	
	// Calculate threshold vectors
	t.XAxis = thresholdX * multiple;
	t.YAxis = thresholdY * multiple;
	t.ZAxis = thresholdZ * multiple;
    } else
    {
	// No threshold
	t.XAxis = 0;
	t.YAxis = 0;
	t.ZAxis = 0;
    }

    // Remember old threshold value
    actualThreshold = multiple;
}

// Write 8-bit to register
void L3G4200D::writeRegister8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(L3G4200D_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(reg);
	Wire.write(value);
    #else
	Wire.send(reg);
	Wire.send(value);
    #endif
    Wire.endTransmission();
}

// Fast read 8-bit from register
uint8_t L3G4200D::fastRegister8(uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(L3G4200D_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(reg);
    #else
	Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(L3G4200D_ADDRESS);
    Wire.requestFrom(L3G4200D_ADDRESS, 1);
    #if ARDUINO >= 100
	value = Wire.read();
    #else
	value = Wire.receive();
    #endif;
    Wire.endTransmission();

    return value;
}

// Read 8-bit from register
uint8_t L3G4200D::readRegister8(uint8_t reg)
{
    uint8_t value;

    Wire.beginTransmission(L3G4200D_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(reg);
    #else
	Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(L3G4200D_ADDRESS);
    Wire.requestFrom(L3G4200D_ADDRESS, 1);
    while(!Wire.available()) {};
    #if ARDUINO >= 100
	value = Wire.read();
    #else
	value = Wire.receive();
    #endif;
    Wire.endTransmission();

    return value;
}

// L3G4200D Temperature sensor output change vs temperature: -1digit/degrCelsius (data representation: 2's complement).
// Value represents difference respect to a reference not specified value.
// So temperature sensor can be used to measure temperature variations: temperarture sensor isn't suitable to return absolute temperatures measures.
// If you run two sequential measures and differentiate them you can get temperature variation.
// This also means that two devices in the same temp conditions can return different outputs.
// Finally, you can use this info to compensate drifts due to temperature changes.
uint8_t L3G4200D::readTemperature(void)
{
    return readRegister8(L3G4200D_OUT_TEMP);
}

// Read raw values
Vector L3G4200D::readRaw()
{
    Wire.beginTransmission(L3G4200D_ADDRESS);
    #if ARDUINO >= 100
	Wire.write(L3G4200D_OUT_X_L | (1 << 7)); 
    #else
	Wire.send(L3G4200D_OUT_X_L | (1 << 7)); 
    #endif
    Wire.endTransmission();
    Wire.requestFrom(L3G4200D_ADDRESS, 6);

    while (Wire.available() < 6);

    #if ARDUINO >= 100
	uint8_t xla = Wire.read();
	uint8_t xha = Wire.read();
	uint8_t yla = Wire.read();
	uint8_t yha = Wire.read();
	uint8_t zla = Wire.read();
	uint8_t zha = Wire.read();
    #else
	uint8_t xla = Wire.receive();
	uint8_t xha = Wire.receive();
	uint8_t yla = Wire.receive();
	uint8_t yha = Wire.receive();
	uint8_t zla = Wire.receive();
	uint8_t zha = Wire.receive();
    #endif

    r.XAxis = xha << 8 | xla;
    r.YAxis = yha << 8 | yla;
    r.ZAxis = zha << 8 | zla;

    return r;
}

// Read normalized values
Vector L3G4200D::readNormalize()
{
    readRaw();

    if (useCalibrate)
    {
	n.XAxis = (r.XAxis - d.XAxis) * dpsPerDigit;
	n.YAxis = (r.YAxis - d.YAxis) * dpsPerDigit;
	n.ZAxis = (r.ZAxis - d.ZAxis) * dpsPerDigit;
    } else
    {
	n.XAxis = r.XAxis * dpsPerDigit;
	n.YAxis = r.YAxis * dpsPerDigit;
	n.ZAxis = r.ZAxis * dpsPerDigit;
    }

    if (actualThreshold > 0)
    {
	if (abs(n.XAxis) < t.XAxis) n.XAxis = 0;
	if (abs(n.YAxis) < t.YAxis) n.YAxis = 0;
	if (abs(n.ZAxis) < t.ZAxis) n.ZAxis = 0;
    }

    return n;
}
