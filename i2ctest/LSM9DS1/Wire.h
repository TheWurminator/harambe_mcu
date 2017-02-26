//This is a fake wrapper for Wire.h in Arduino since we're not using that

//Needed for all of the i2c functions
#include <ti/drivers/I2C.h>
//Needed for the system functions
#include <xdc/std.h>
#include <xdc/runtime/System.h>
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include "../Board.h"
#define BUFFER_LENGTH 32
#define WIRE_HAS_END 1


class TwoWire{
public:
    TwoWire();
    void (*resetPointer)();
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;
    uint8_t txaddress;
    uint8_t transmitting;
    uint8_t txBufferIndex;
    uint8_t txBufferLength;
    uint8_t txBuffer[BUFFER_LENGTH];
    uint8_t rxBuffer[BUFFER_LENGTH];
    uint8_t rxBufferIndex;
    //Just pull the max every time
    uint8_t rxBufferLength;
    void begin(void (*resetPointer)());
    void beginTransmission(uint8_t address);
    uint8_t write(uint8_t something);
    uint8_t endTransmission(bool tf = true);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t read();
};

extern TwoWire Wire;
