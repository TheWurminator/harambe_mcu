#include "LSM9DS1.h"
#include "Wire2.h"

Wire TheWire;

#define SENSITIVITY_ACCELEROMETER_2  0.000061
#define SENSITIVITY_ACCELEROMETER_4  0.000122
#define SENSITIVITY_ACCELEROMETER_8  0.000244
#define SENSITIVITY_ACCELEROMETER_16 0.000732
#define SENSITIVITY_GYROSCOPE_245    0.00875
#define SENSITIVITY_GYROSCOPE_500    0.0175
#define SENSITIVITY_GYROSCOPE_2000   0.07
#define SENSITIVITY_MAGNETOMETER_4   0.00014
#define SENSITIVITY_MAGNETOMETER_8   0.00029
#define SENSITIVITY_MAGNETOMETER_12  0.00043
#define SENSITIVITY_MAGNETOMETER_16  0.00058
#define LSM9DS1_M   0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

void LSM9DS1Init(LSM9DS1 * thing){
    //Making this C compatible
    thing->settings.device.commInterface = IMU_MODE_I2C;
    thing->settings.device.mAddress = LSM9DS1_M;
    thing->settings.device.agAddress = LSM9DS1_AG;


    thing->settings.gyro.enabled = true;
    thing->settings.gyro.enableX = true;
    thing->settings.gyro.enableY = true;
    thing->settings.gyro.enableZ = true;
    // gyro scale can be 245, 500, or 2000
    thing->settings.gyro.scale = 245;
    // gyro sample rate: value between 1-6
    // 1 = 14.9    4 = 238
    // 2 = 59.5    5 = 476
    // 3 = 119     6 = 952
    thing->settings.gyro.sampleRate = 6;
    // gyro cutoff frequency: value between 0-3
    // Actual value of cutoff frequency depends
    // on sample rate.
    thing->settings.gyro.bandwidth = 0;
    thing->settings.gyro.lowPowerEnable = false;
    thing->settings.gyro.HPFEnable = false;
    // Gyro HPF cutoff frequency: value between 0-9
    // Actual value depends on sample rate. Only applies
    // if gyroHPFEnable is true.
    thing->settings.gyro.HPFCutoff = 0;
    thing->settings.gyro.flipX = false;
    thing->settings.gyro.flipY = false;
    thing->settings.gyro.flipZ = false;
    thing->settings.gyro.orientation = 0;
    thing->settings.gyro.latchInterrupt = true;

    thing->settings.accel.enabled = true;
    thing->settings.accel.enableX = true;
    thing->settings.accel.enableY = true;
    thing->settings.accel.enableZ = true;
    // accel scale can be 2, 4, 8, or 16
    thing->settings.accel.scale = 2;
    // accel sample rate can be 1-6
    // 1 = 10 Hz    4 = 238 Hz
    // 2 = 50 Hz    5 = 476 Hz
    // 3 = 119 Hz   6 = 952 Hz
    thing->settings.accel.sampleRate = 6;
    // Accel cutoff freqeuncy can be any value between -1 - 3.
    // -1 = bandwidth determined by sample rate
    // 0 = 408 Hz   2 = 105 Hz
    // 1 = 211 Hz   3 = 50 Hz
    thing->settings.accel.bandwidth = -1;
    thing->settings.accel.highResEnable = false;
    // accelHighResBandwidth can be any value between 0-3
    // LP cutoff is set to a factor of sample rate
    // 0 = ODR/50    2 = ODR/9
    // 1 = ODR/100   3 = ODR/400
    thing->settings.accel.highResBandwidth = 0;

    thing->settings.mag.enabled = true;
    // mag scale can be 4, 8, 12, or 16
    thing->settings.mag.scale = 4;
    // mag data rate can be 0-7
    // 0 = 0.625 Hz  4 = 10 Hz
    // 1 = 1.25 Hz   5 = 20 Hz
    // 2 = 2.5 Hz    6 = 40 Hz
    // 3 = 5 Hz      7 = 80 Hz
    thing->settings.mag.sampleRate = 7;
    thing->settings.mag.tempCompensationEnable = false;
    // magPerformance can be any value between 0-3
    // 0 = Low power mode      2 = high performance
    // 1 = medium performance  3 = ultra-high performance
    thing->settings.mag.XYPerformance = 3;
    thing->settings.mag.ZPerformance = 3;
    thing->settings.mag.lowPowerEnable = false;
    // magOperatingMode can be 0-2
    // 0 = continuous conversion
    // 1 = single-conversion
    // 2 = power down
    thing->settings.mag.operatingMode = 0;

    thing->settings.temp.enabled = true;
    uint8_t i = 0;
    for (i=0; i<3; i++)
    {
        thing->gBias[i] = 0;
        thing->aBias[i] = 0;
        thing->mBias[i] = 0;
        thing->gBiasRaw[i] = 0;
        thing->aBiasRaw[i] = 0;
        thing->mBiasRaw[i] = 0;
    }
    thing->_autoCalc = false;
}

uint16_t LSM9DS1begin(void (* rstPtr)(), LSM9DS1 * thing)
{
    //! Todo: don't use _xgAddress or _mAddress, duplicating memory
    thing->_xgAddress = thing->settings.device.agAddress;
    thing->_mAddress = thing->settings.device.mAddress;
    LSM9DS1constrainScales(thing);
    // Once we have the scale values, we can calculate the resolution
    // of each sensor. That's what these functions are for. One for each sensor
    LSM9DS1calcgRes(thing); // Calculate DPS / ADC tick, stored in gRes variable
    LSM9DS1calcmRes(thing); // Calculate Gs / ADC tick, stored in mRes variable
    LSM9DS1calcaRes(thing); // Calculate g / ADC tick, stored in aRes variable

    // Now, initialize our hardware interface.
    initI2C(rstPtr);  // Initialize I2C
    System_printf("i2c is initialized\n");
    System_flush();

    // To verify communication, we can read from the WHO_AM_I register of
    // each device. Store those in a variable so we can return them.
    uint8_t mTest = LSM9DS1mReadByte(WHO_AM_I_M);      // Read the gyro WHO_AM_I
    uint8_t xgTest = LSM9DS1xgReadByte(WHO_AM_I_XG);   // Read the accel/mag WHO_AM_I
    uint16_t whoAmICombined = (xgTest << 8) | mTest;
    if (whoAmICombined != ((WHO_AM_I_AG_RSP << 8) | WHO_AM_I_M_RSP)){
        return 0;}

    // Gyro initialization stuff:
    LSM9DS1initGyro(thing); // This will "turn on" the gyro. Setting up interrupts, etc.

    // Accelerometer initialization stuff:
    LSM9DS1initAccel(thing); // "Turn on" all axes of the accel. Set up interrupts, etc.

    // Magnetometer initialization stuff:
    LSM9DS1initMag(thing); // "Turn on" all axes of the mag. Set up interrupts, etc.

    // Once everything is initialized, return the WHO_AM_I registers we read:
    return whoAmICombined;
}

void LSM9DS1constrainScales(LSM9DS1 * thing)
{
    if ((thing->settings.gyro.scale != 245) && (thing->settings.gyro.scale != 500) &&
        (thing->settings.gyro.scale != 2000))
    {
        thing->settings.gyro.scale = 245;
    }

    if ((thing->settings.accel.scale != 2) && (thing->settings.accel.scale != 4) &&
        (thing->settings.accel.scale != 8) && (thing->settings.accel.scale != 16))
    {
        thing->settings.accel.scale = 2;
    }

    if ((thing->settings.mag.scale != 4) && (thing->settings.mag.scale != 8) &&
        (thing->settings.mag.scale != 12) && (thing->settings.mag.scale != 16))
    {
        thing->settings.mag.scale = 4;
    }
}

void LSM9DS1initMag(LSM9DS1 * thing)
{
    uint8_t tempRegValue = 0;

    // CTRL_REG1_M (Default value: 0x10)
    // [TEMP_COMP][OM1][OM0][DO2][DO1][DO0][0][ST]
    // TEMP_COMP - Temperature compensation
    // OM[1:0] - X & Y axes op mode selection
    //  00:low-power, 01:medium performance
    //  10: high performance, 11:ultra-high performance
    // DO[2:0] - Output data rate selection
    // ST - Self-test enable
    if (thing->settings.mag.tempCompensationEnable) tempRegValue |= (1<<7);
    tempRegValue |= (thing->settings.mag.XYPerformance & 0x3) << 5;
    tempRegValue |= (thing->settings.mag.sampleRate & 0x7) << 2;

    LSM9DS1mWriteByte(CTRL_REG1_M, tempRegValue, thing);

    // CTRL_REG2_M (Default value 0x00)
    // [0][FS1][FS0][0][REBOOT][SOFT_RST][0][0]
    // FS[1:0] - Full-scale configuration
    // REBOOT - Reboot memory content (0:normal, 1:reboot)
    // SOFT_RST - Reset config and user registers (0:default, 1:reset)
    tempRegValue = 0;
    switch (thing->settings.mag.scale)
    {
    case 8:
        tempRegValue |= (0x1 << 5);
        break;
    case 12:
        tempRegValue |= (0x2 << 5);
        break;
    case 16:
        tempRegValue |= (0x3 << 5);
        break;
    // Otherwise we'll default to 4 gauss (00)
    }
    LSM9DS1mWriteByte(CTRL_REG2_M, tempRegValue, thing); // +/-4Gauss

    // CTRL_REG3_M (Default value: 0x03)
    // [I2C_DISABLE][0][LP][0][0][SIM][MD1][MD0]
    // I2C_DISABLE - Disable I2C interace (0:enable, 1:disable)
    // LP - Low-power mode cofiguration (1:enable)
    // SIM - SPI mode selection (0:write-only, 1:read/write enable)
    // MD[1:0] - Operating mode
    //  00:continuous conversion, 01:single-conversion,
    //  10,11: Power-down
    tempRegValue = 0;
    if (thing->settings.mag.lowPowerEnable) tempRegValue |= (1<<5);
    tempRegValue |= (thing->settings.mag.operatingMode & 0x3);
    LSM9DS1mWriteByte(CTRL_REG3_M, tempRegValue, thing); // Continuous conversion mode

    // CTRL_REG4_M (Default value: 0x00)
    // [0][0][0][0][OMZ1][OMZ0][BLE][0]
    // OMZ[1:0] - Z-axis operative mode selection
    //  00:low-power mode, 01:medium performance
    //  10:high performance, 10:ultra-high performance
    // BLE - Big/little endian data
    tempRegValue = 0;
    tempRegValue = (thing->settings.mag.ZPerformance & 0x3) << 2;
    LSM9DS1mWriteByte(CTRL_REG4_M, tempRegValue, thing);

    // CTRL_REG5_M (Default value: 0x00)
    // [0][BDU][0][0][0][0][0][0]
    // BDU - Block data update for magnetic data
    //  0:continuous, 1:not updated until MSB/LSB are read
    tempRegValue = 0;
    LSM9DS1mWriteByte(CTRL_REG5_M, tempRegValue, thing);
}

void LSM9DS1mWriteByte(uint8_t subAddress, uint8_t data, LSM9DS1 * thing)
{
    // Whether we're using I2C or SPI, write a byte using the
    // accelerometer-specific I2C address or SPI CS pin.

    LSM9DS1I2CwriteByte(thing->_mAddress, subAddress, data, thing);

}

// Wire.h read and write protocols
void LSM9DS1I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data, LSM9DS1 * thing)
{
    WirebeginTransmission(address, &TheWire);  // Initialize the Tx buffer
    Wirewrite(subAddress, &TheWire);           // Put slave register address in Tx buffer
    Wirewrite(data, &TheWire);                 // Put data in Tx buffer
    WireendTransmission(0,&TheWire);           // Send the Tx buffer
}

void LSM9DS1initAccel(LSM9DS1 * thing)
{
    uint8_t tempRegValue = 0;

    //  CTRL_REG5_XL (0x1F) (Default value: 0x38)
    //  [DEC_1][DEC_0][Zen_XL][Yen_XL][Zen_XL][0][0][0]
    //  DEC[0:1] - Decimation of accel data on OUT REG and FIFO.
    //      00: None, 01: 2 samples, 10: 4 samples 11: 8 samples
    //  Zen_XL - Z-axis output enabled
    //  Yen_XL - Y-axis output enabled
    //  Xen_XL - X-axis output enabled
    if (thing->settings.accel.enableZ) tempRegValue |= (1<<5);
    if (thing->settings.accel.enableY) tempRegValue |= (1<<4);
    if (thing->settings.accel.enableX) tempRegValue |= (1<<3);

    LSM9DS1xgWriteByte(CTRL_REG5_XL, tempRegValue);

    // CTRL_REG6_XL (0x20) (Default value: 0x00)
    // [ODR_XL2][ODR_XL1][ODR_XL0][FS1_XL][FS0_XL][BW_SCAL_ODR][BW_XL1][BW_XL0]
    // ODR_XL[2:0] - Output data rate & power mode selection
    // FS_XL[1:0] - Full-scale selection
    // BW_SCAL_ODR - Bandwidth selection
    // BW_XL[1:0] - Anti-aliasing filter bandwidth selection
    tempRegValue = 0;
    // To disable the accel, set the sampleRate bits to 0.
    if (thing->settings.accel.enabled)
    {
        tempRegValue |= (thing->settings.accel.sampleRate & 0x07) << 5;
    }
    switch (thing->settings.accel.scale)
    {
        case 4:
            tempRegValue |= (0x2 << 3);
            break;
        case 8:
            tempRegValue |= (0x3 << 3);
            break;
        case 16:
            tempRegValue |= (0x1 << 3);
            break;
        // Otherwise it'll be set to 2g (0x0 << 3)
    }
    if (thing->settings.accel.bandwidth >= 0)
    {
        tempRegValue |= (1<<2); // Set BW_SCAL_ODR
        tempRegValue |= (thing->settings.accel.bandwidth & 0x03);
    }

    LSM9DS1xgWriteByte(CTRL_REG6_XL, tempRegValue, thing);

    // CTRL_REG7_XL (0x21) (Default value: 0x00)
    // [HR][DCF1][DCF0][0][0][FDS][0][HPIS1]
    // HR - High resolution mode (0: disable, 1: enable)
    // DCF[1:0] - Digital filter cutoff frequency
    // FDS - Filtered data selection
    // HPIS1 - HPF enabled for interrupt function
    tempRegValue = 0;
    if (thing->settings.accel.highResEnable)
    {
        tempRegValue |= (1<<7); // Set HR bit
        tempRegValue |= (thing->settings.accel.highResBandwidth & 0x3) << 5;
    }
    LSM9DS1xgWriteByte(CTRL_REG7_XL, tempRegValue);
}
void LSM9DS1xgWriteByte(uint8_t subAddress, uint8_t data, LSM9DS1 * thing)
{
    // Whether we're using I2C or SPI, write a byte using the
    // gyro-specific I2C address or SPI CS pin.
    LSM9DS1I2CwriteByte(thing->_xgAddress, subAddress, data, thing);

}

void LSM9DS1initGyro(LSM9DS1 * thing)
{
    uint8_t tempRegValue = 0;

    // CTRL_REG1_G (Default value: 0x00)
    // [ODR_G2][ODR_G1][ODR_G0][FS_G1][FS_G0][0][BW_G1][BW_G0]
    // ODR_G[2:0] - Output data rate selection
    // FS_G[1:0] - Gyroscope full-scale selection
    // BW_G[1:0] - Gyroscope bandwidth selection

    // To disable gyro, set sample rate bits to 0. We'll only set sample
    // rate if the gyro is enabled.
    if (thing->settings.gyro.enabled)
    {
        tempRegValue = (thing->settings.gyro.sampleRate & 0x07) << 5;
    }
    switch (thing->settings.gyro.scale)
    {
        case 500:
            tempRegValue |= (0x1 << 3);
            break;
        case 2000:
            tempRegValue |= (0x3 << 3);
            break;
        // Otherwise we'll set it to 245 dps (0x0 << 4)
    }
    tempRegValue |= (thing->settings.gyro.bandwidth & 0x3);

    LSM9DS1xgWriteByte(CTRL_REG1_G, tempRegValue, thing);

    // CTRL_REG2_G (Default value: 0x00)
    // [0][0][0][0][INT_SEL1][INT_SEL0][OUT_SEL1][OUT_SEL0]
    // INT_SEL[1:0] - INT selection configuration
    // OUT_SEL[1:0] - Out selection configuration
    LSM9DS1xgWriteByte(CTRL_REG2_G, 0x00, thing);

    // CTRL_REG3_G (Default value: 0x00)
    // [LP_mode][HP_EN][0][0][HPCF3_G][HPCF2_G][HPCF1_G][HPCF0_G]
    // LP_mode - Low-power mode enable (0: disabled, 1: enabled)
    // HP_EN - HPF enable (0:disabled, 1: enabled)
    // HPCF_G[3:0] - HPF cutoff frequency
    tempRegValue = thing->settings.gyro.lowPowerEnable ? (1<<7) : 0;
    if (thing->settings.gyro.HPFEnable)
    {
        tempRegValue |= (1<<6) | (thing->settings.gyro.HPFCutoff & 0x0F);
    }
    LSM9DS1xgWriteByte(CTRL_REG3_G, tempRegValue, thing);

    // CTRL_REG4 (Default value: 0x38)
    // [0][0][Zen_G][Yen_G][Xen_G][0][LIR_XL1][4D_XL1]
    // Zen_G - Z-axis output enable (0:disable, 1:enable)
    // Yen_G - Y-axis output enable (0:disable, 1:enable)
    // Xen_G - X-axis output enable (0:disable, 1:enable)
    // LIR_XL1 - Latched interrupt (0:not latched, 1:latched)
    // 4D_XL1 - 4D option on interrupt (0:6D used, 1:4D used)
    tempRegValue = 0;
    if (thing->settings.gyro.enableZ) tempRegValue |= (1<<5);
    if (thing->settings.gyro.enableY) tempRegValue |= (1<<4);
    if (thing->settings.gyro.enableX) tempRegValue |= (1<<3);
    if (thing->settings.gyro.latchInterrupt) tempRegValue |= (1<<1);
    LSM9DS1xgWriteByte(CTRL_REG4, tempRegValue, thing);

    // ORIENT_CFG_G (Default value: 0x00)
    // [0][0][SignX_G][SignY_G][SignZ_G][Orient_2][Orient_1][Orient_0]
    // SignX_G - Pitch axis (X) angular rate sign (0: positive, 1: negative)
    // Orient [2:0] - Directional user orientation selection
    tempRegValue = 0;
    if (thing->settings.gyro.flipX) tempRegValue |= (1<<5);
    if (thing->settings.gyro.flipY) tempRegValue |= (1<<4);
    if (thing->settings.gyro.flipZ) tempRegValue |= (1<<3);
    LSM9DS1xgWriteByte(ORIENT_CFG_G, tempRegValue, thing);
}

void initI2C(void (*rstPtr)())
{
    Wirebegin(rstPtr, &TheWire);   // Initialize I2C library
}

void LSM9DS1calcgRes(LSM9DS1 * thing)
{
    switch (thing->settings.gyro.scale)
    {
    case 245:
        thing->gRes = SENSITIVITY_GYROSCOPE_245;
        break;
    case 500:
        thing->gRes = SENSITIVITY_GYROSCOPE_500;
        break;
    case 2000:
        thing->gRes = SENSITIVITY_GYROSCOPE_2000;
        break;
    default:
        break;
    }
}

void LSM9DS1calcmRes(LSM9DS1 * thing)
{
    switch (thing->settings.mag.scale)
    {
    case 4:
        thing->mRes = SENSITIVITY_MAGNETOMETER_4;
        break;
    case 8:
        thing->mRes = SENSITIVITY_MAGNETOMETER_8;
        break;
    case 12:
        thing->mRes = SENSITIVITY_MAGNETOMETER_12;
        break;
    case 16:
        thing->mRes = SENSITIVITY_MAGNETOMETER_16;
        break;
    }
}

void LSM9DS1calcaRes(LSM9DS1 * thing)
{
    switch (thing->settings.accel.scale)
    {
    case 2:
        thing->aRes = SENSITIVITY_ACCELEROMETER_2;
        break;
    case 4:
        thing->aRes = SENSITIVITY_ACCELEROMETER_4;
        break;
    case 8:
        thing->aRes = SENSITIVITY_ACCELEROMETER_8;
        break;
    case 16:
        thing->aRes = SENSITIVITY_ACCELEROMETER_16;
        break;
    default:
        break;
    }
}

uint8_t LSM9DS1mReadBytes(uint8_t subAddress, uint8_t * dest, uint8_t count, LSM9DS1 * thing)
{
    // Whether we're using I2C or SPI, read multiple bytes using the
    // accelerometer-specific I2C address or SPI CS pin.

    return I2CreadBytes(thing->_mAddress, subAddress, dest, count, thing);
}

uint8_t LSM9DS1I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count)
{
    uint8_t retVal;
    WirebeginTransmission(address, &TheWire);      // Initialize the Tx buffer
    // Next send the register to be read. OR with 0x80 to indicate multi-read.
    Wirewrite(subAddress | 0x80, &TheWire);        // Put slave register address in Tx buffer


    retVal = WirerequestFrom(address, count, &TheWire);  // Read bytes from slave register address
    if (retVal != count)
        return 0;

    //Now we know that everything is being requested properly
    retVal = WireendTransmission(0, &TheWire); // Send Tx buffer, send a restart to keep connection alive
    uint8_t i = 0;
    for (i=0; i<count;)
        dest[i++] = Wireread(&TheWire);

    return count;
}

uint8_t LSM9DS1mReadByte(uint8_t subAddress, LSM9DS1 * thing)
{
    // Whether we're using I2C or SPI, read a byte using the
    // accelerometer-specific I2C address or SPI CS pin.

    return LSM9DS1I2CreadByte(thing->_mAddress, subAddress, thing);

}

uint8_t LSM9DS1I2CreadByte(uint8_t address, uint8_t subAddress, LSM9DS1 * thing)
{
    uint8_t data; // `data` will store the register data

    WirebeginTransmission(address, &TheWire);         // Initialize the Tx buffer
    Wirewrite(subAddress, &TheWire);                  // Put slave register address in Tx buffer
    WirerequestFrom(address, (uint8_t) 1, &TheWire);  // Read one byte from slave register address
//    System_printf("Address of 0x%x\n", address );
//    System_flush();
    //Moved the end transmission down here
    WireendTransmission(0, &TheWire);             // Send the Tx buffer, but send a restart to keep connection alive
//    System_printf("Transmission was completed\n");
//    System_flush();
    data = Wireread(&TheWire);
    // Fill Rx buffer with result
//    System_printf("The val is 0x%x\n", data);
//    System_flush();
    return data;                             // Return data read from slave register
}
