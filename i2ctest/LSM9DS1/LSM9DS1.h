/*
 * LSM9DS1.h
 *
 *  Created on: Mar 24, 2017
 *      Author: BeetleJuice
 */

#ifndef LSM9DS1_LSM9DS1_H_
#define LSM9DS1_LSM9DS1_H_

#include "LSM9DS1_Types.h"
#include "LSM9DS1_Registers.h"

#define LSM9DS1_AG_ADDR(sa0)    ((sa0) == 0 ? 0x6A : 0x6B)
#define LSM9DS1_M_ADDR(sa1)     ((sa1) == 0 ? 0x1C : 0x1E)

enum lsm9ds1_axis {
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
    ALL_AXIS
};

typedef struct{
    IMUSettings settings;

    // We'll store the gyro, accel, and magnetometer readings in a series of
    // public class variables. Each sensor gets three variables -- one for each
    // axis. Call readGyro(), readAccel(), and readMag() first, before using
    // these variables!
    // These values are the RAW signed 16-bit readings from the sensors.
    int16_t gx, gy, gz; // x, y, and z axis readings of the gyroscope
    int16_t ax, ay, az; // x, y, and z axis readings of the accelerometer
    int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer
    int16_t temperature; // Chip temperature
    float gBias[3], aBias[3], mBias[3];
    int16_t gBiasRaw[3], aBiasRaw[3], mBiasRaw[3];
    // x_mAddress and gAddress store the I2C address or SPI chip select pin
    // for each sensor.
    uint8_t _mAddress, _xgAddress;

    // gRes, aRes, and mRes store the current resolution for each sensor.
    // Units of these values would be DPS (or g's or Gs's) per ADC tick.
    // This value is calculated as (sensor scale) / (2^15).
    float gRes, aRes, mRes;

    // _autoCalc keeps track of whether we're automatically subtracting off
    // accelerometer and gyroscope bias calculated in calibrate().
    uint8_t _autoCalc;
}LSM9DS1;

// Function Prototypes
void LSM9DS1Init(LSM9DS1 * thing);
uint16_t LSM9DS1begin(void (* rstPtr)(), LSM9DS1 * thing);
void LSM9DS1constrainScales(LSM9DS1 * thing);
void LSM9DS1initMag(LSM9DS1 * thing);
void LSM9DS1mWriteByte(uint8_t subAddress, uint8_t data, LSM9DS1 * thing);
void LSM9DS1I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data, LSM9DS1 * thing);
void LSM9DS1initAccel(LSM9DS1 * thing);
void LSM9DS1xgWriteByte(uint8_t subAddress, uint8_t data, LSM9DS1 * thing);
void LSM9DS1initGyro(LSM9DS1 * thing);
void initI2C(void (*rstPtr)());
void LSM9DS1calcgRes(LSM9DS1 * thing);
void LSM9DS1calcmRes(LSM9DS1 * thing);
void LSM9DS1calcaRes(LSM9DS1 * thing);
uint8_t LSM9DS1xgReadByte(uint8_t subAddress, LSM9DS1 * thing);
uint8_t LSM9DS1mReadBytes(uint8_t subAddress, uint8_t * dest, uint8_t count, LSM9DS1 * thing);
uint8_t LSM9DS1I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);
uint8_t LSM9DS1mReadByte(uint8_t subAddress, LSM9DS1 * thing);
uint8_t LSM9DS1I2CreadByte(uint8_t address, uint8_t subAddress, LSM9DS1 * thing);
void LSM9DS1readAccel(LSM9DS1 * thing);
void LSM9DS1readMag(LSM9DS1 * thing);
void LSM9DS1readGyro(LSM9DS1 * thing);
float LSM9DS1calcGyro(int16_t gyro, LSM9DS1 * thing);
float LSM9DS1calcAccel(int16_t accel, LSM9DS1 * thing);
float LSM9DS1calcMag(int16_t mag, LSM9DS1 * thing);
uint8_t LSM9DS1xgReadBytes(uint8_t subAddress, uint8_t * dest, uint8_t count, LSM9DS1 * thing);



#endif /* LSM9DS1_LSM9DS1_H_ */
