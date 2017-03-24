//This is going
#include "Wire2.h"
//Needed for the system functions
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>


void Wirebegin(void (*resetPointer)(), Wire * thing){
    //Needed to pass this function pointer down to reset everything
   resetPointer = thing->rstPtr;
   I2C_Params i2cParams = thing->i2cParams;
   I2C_Params_init(&i2cParams);
   i2cParams.bitRate = I2C_400kHz;
   i2cParams.transferMode = I2C_MODE_BLOCKING; //We want it to block on a semaphore
   System_printf("Set up the params\n");
   System_flush();
   thing->i2c = I2C_open(Board_I2C0, &i2cParams);
   if (thing->i2c == NULL) { //Kill the whole thing
       System_abort("Error Initializing I2C\n");
       System_flush();
   }
   else {
       System_printf("I2C Initialized!\n");
       System_flush();
   }
}

void WirebeginTransmission(uint8_t address, Wire * thing){
    thing->i2cTransaction.slaveAddress = address;
    thing->txBufferIndex = 0;
    thing->txBufferLength = 0;
    thing->rxBufferIndex = 0;
    thing->rxBufferLength = 1;
}

uint8_t Wirewrite(uint8_t data, Wire * thing){
    //Add data to the buffer
    thing->txBuffer[thing->txBufferIndex] = data;
    //Increment the counter and the length
    thing->txBufferLength = ++thing->txBufferIndex;
    return 0;
}

uint8_t WireendTransmission(uint8_t tf, Wire * thing){
    I2C_Transaction i2cTransaction = thing->i2cTransaction;
    i2cTransaction.readBuf = thing->rxBuffer;
    i2cTransaction.writeBuf = thing->txBuffer;
    i2cTransaction.readCount = thing->rxBufferLength;
    i2cTransaction.writeCount = thing->txBufferLength;
//    System_printf("Slave addr is : 0x%x\n", i2cTransaction.slaveAddress);
//    System_printf("i2c addr is : 0x%x\n", i2c);
//    System_printf("transaction addr is : 0x%x\n", &i2cTransaction);
//    System_flush();
    while(!I2C_transfer(thing->i2c, &i2cTransaction)){
       System_printf("THIS SHIT SUCKS\n");
       System_flush();
       //This is the jankest thing I've ever done
       thing->rstPtr();
    }
   i2cTransaction.readCount = 0;
   thing->txBufferLength = 0;
   thing->txBufferIndex = 0;
   return 0;
}

uint8_t WirerequestFrom(uint8_t first, uint8_t quantity, Wire * thing){
    thing->rxBufferLength = quantity;
    return quantity;
}

uint8_t Wireread(Wire * thing){
    if(thing->rxBufferIndex < thing->rxBufferLength){
        return thing->rxBuffer[thing->rxBufferIndex++];
    }
    else return 1;
}
