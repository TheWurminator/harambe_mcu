//This is going
#include "Wire.h"
//Needed for the system functions
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>

TwoWire::TwoWire()
{
}
//Purpose is to get i2c initialized
void TwoWire::begin(void (*rstPtr)()){
    //Needed to pass this function pointer down to reset everything
    resetPointer = rstPtr;
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING; //We want it to block on a semaphore
    System_printf("Set up the params\n");
    System_flush();
    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) { //Kill the whole thing
        System_abort("Error Initializing I2C\n");
        System_flush();
    }
    else {
        System_printf("I2C Initialized!\n");
        System_flush();
    }
}

//Initializing TX Buffer
void TwoWire::beginTransmission(uint8_t address){
    i2cTransaction.slaveAddress = address;
    txBufferIndex = 0;
    txBufferLength = 0;
    rxBufferIndex = 0;
    rxBufferLength = 1;
}

//Send the TX Buffer
uint8_t TwoWire::endTransmission(bool tf){
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.readCount = rxBufferLength;
    i2cTransaction.writeCount = txBufferLength;
//    System_printf("Slave addr is : 0x%x\n", i2cTransaction.slaveAddress);
//    System_printf("i2c addr is : 0x%x\n", i2c);
//    System_printf("transaction addr is : 0x%x\n", &i2cTransaction);
//    System_flush();
    while(!I2C_transfer(i2c, &i2cTransaction)){
       System_printf("THIS SHIT SUCKS\n");
       System_flush();
       //This is the jankest thing I've ever done
       resetPointer();
    }
   i2cTransaction.readCount = 0;
   txBufferLength = 0;
   txBufferIndex = 0;
   return 0;
}

//Used to fill up the tx buffer byte by byte
uint8_t TwoWire::write(uint8_t data){
    //Add data to the buffer
    txBuffer[txBufferIndex] = data;
    //Increment the counter and the length
    txBufferLength = ++txBufferIndex;
    return 0;
}

//Make it think that we actually read after the fact
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    rxBufferLength = quantity;
    return quantity;
}

//Return a byte from the rxbuffer
uint8_t TwoWire::read(){
    if(rxBufferIndex < rxBufferLength){
        return rxBuffer[rxBufferIndex++];
    }
    else return 1;
}

TwoWire Wire = TwoWire();
