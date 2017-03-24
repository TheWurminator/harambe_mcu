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

typedef struct{
    void (*rstPtr)();
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

}Wire;

void Wirebegin(void (*resetPointer)(), Wire * thing);
void WirebeginTransmission(uint8_t address, Wire * thing);
uint8_t Wirewrite(uint8_t something, Wire * thing);
uint8_t WireendTransmission(uint8_t tf, Wire * thing);
uint8_t WirerequestFrom(uint8_t, uint8_t, Wire *);
uint8_t Wireread(Wire * thing);
