/*
L3G4200D.cpp - Class file for the L3G4200D Triple Axis Gyroscope Arduino Library.

Version: 1.0.0
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

#include <L3G4200D.h>
#include <Wire.h>
#include <math.h>

#define GYR_ADDRESS (0xD2 >> 1)

boolean L3G4200D::begin(int scale)
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

    // Enable all axis and setup normal mode (0b00001111)
    writeReg(L3G4200D_CTRL_REG1, 0x0F);

    // Disable high pass filter (0b00000000)
    writeReg(L3G4200D_CTRL_REG2, 0x00);

    // Generata data ready interrupt on INT2 (0b00001000)
    writeReg(L3G4200D_CTRL_REG3, 0x08);

    // Set full scale selection in continous mode
    if (scale == L3G4200D_250DPS)
    {
	// 250 dps (0b00000000)
	writeReg(L3G4200D_CTRL_REG4, 0x00);
	dpsPerDigit = .00875f;
    } else
    if (scale == L3G4200D_500DPS)
    {
	// 500 dps (0b00010000)
	writeReg(L3G4200D_CTRL_REG4, 0x10);
	dpsPerDigit = .0175f;
    } else
    if (scale == L3G4200D_2000DPS)
    {
	// 2000 dps (0b00100000)
	writeReg(L3G4200D_CTRL_REG4, 0x20);
	dpsPerDigit = .07f;
    } else
    {
	return false;
    }

    // Boot in normal mode, disable FIFO, HPF disabled (0b00000000) 
    writeReg(L3G4200D_CTRL_REG5, 0x00);

    return true;
}

void L3G4200D::calibrate(int samples)
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
    for (int i = 0; i < samples; ++i)
    {
	readRaw();
	sumX += r.XAxis;
	sumY += r.YAxis;
	sumZ += r.ZAxis;

	sigmaX += r.XAxis * r.XAxis;
	sigmaY += r.YAxis * r.YAxis;
	sigmaZ += r.ZAxis * r.ZAxis;
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

void L3G4200D::setThreshold(int multiple)
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

void L3G4200D::writeReg(byte reg, byte value)
{
    Wire.beginTransmission(GYR_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

byte L3G4200D::readReg(byte reg)
{
    byte value;
    Wire.beginTransmission(GYR_ADDRESS);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(GYR_ADDRESS, 1);
    while(!Wire.available()) {};
    value = Wire.read();
    Wire.endTransmission();
    return value;
}

GyroscopeRaw L3G4200D::readRaw()
{
    Wire.beginTransmission(GYR_ADDRESS);

    Wire.write(L3G4200D_OUT_X_L | (1 << 7)); 
    Wire.endTransmission();
    Wire.requestFrom(GYR_ADDRESS, 6);

    while (Wire.available() < 6);

    uint8_t xla = Wire.read();
    uint8_t xha = Wire.read();
    uint8_t yla = Wire.read();
    uint8_t yha = Wire.read();
    uint8_t zla = Wire.read();
    uint8_t zha = Wire.read();

    r.XAxis = xha << 8 | xla;
    r.YAxis = yha << 8 | yla;
    r.ZAxis = zha << 8 | zla;

    return r;
}

GyroscopeNormalize L3G4200D::readNormalize()
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
