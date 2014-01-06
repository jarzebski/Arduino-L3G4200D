/*
L3G4200D.h - Header file for the L3G4200D Triple Axis Gyroscope Arduino Library.

Version: 1.1.0
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

#ifndef L3G4200D_h
#define L3G4200D_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Arduino.h>

#define L3G4200D_ADDRESS       (0xD2 >> 1)

#define L3G4200D_WHO_AM_I      0x0F

#define L3G4200D_CTRL_REG1     0x20
#define L3G4200D_CTRL_REG2     0x21
#define L3G4200D_CTRL_REG3     0x22
#define L3G4200D_CTRL_REG4     0x23
#define L3G4200D_CTRL_REG5     0x24
#define L3G4200D_REFERENCE     0x25
#define L3G4200D_OUT_TEMP      0x26
#define L3G4200D_STATUS_REG    0x27

#define L3G4200D_OUT_X_L       0x28
#define L3G4200D_OUT_X_H       0x29
#define L3G4200D_OUT_Y_L       0x2A
#define L3G4200D_OUT_Y_H       0x2B
#define L3G4200D_OUT_Z_L       0x2C
#define L3G4200D_OUT_Z_H       0x2D

#define L3G4200D_FIFO_CTRL_REG 0x2E
#define L3G4200D_FIFO_SRC_REG  0x2F

#define L3G4200D_INT1_CFG      0x30
#define L3G4200D_INT1_SRC      0x31
#define L3G4200D_INT1_THS_XH   0x32
#define L3G4200D_INT1_THS_XL   0x33
#define L3G4200D_INT1_THS_YH   0x34
#define L3G4200D_INT1_THS_YL   0x35
#define L3G4200D_INT1_THS_ZH   0x36
#define L3G4200D_INT1_THS_ZL   0x37
#define L3G4200D_INT1_DURATION 0x38

struct GyroscopeVector
{
    float XAxis;
    float YAxis;
    float ZAxis;
};

typedef enum
{
    L3G4200D_2000DPS = 0b10,
    L3G4200D_500DPS = 0b01,
    L3G4200D_250DPS = 0b00
} dps_t;

class L3G4200D
{
    public:
	boolean begin(dps_t scale = L3G4200D_2000DPS);
	dps_t getScale(void);
	void calibrate(int samples = 50);
	void setThreshold(int multiple = 0);

	GyroscopeVector readRaw(void);
	GyroscopeVector readNormalize();

    private:
	GyroscopeVector r;
	GyroscopeVector n;
	GyroscopeVector d;
	GyroscopeVector t;

	boolean useCalibrate;
	float actualThreshold;
	float dpsPerDigit;
	float thresholdX;
	float thresholdY;
	float thresholdZ;

	void writeReg(byte reg, byte value);
	byte readReg(byte reg);
	byte fastReg(byte reg);
};

#endif
