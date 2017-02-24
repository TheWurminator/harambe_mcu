//This is going
#include "Wire.h"
//Needed for the system functions
#include <xdc/std.h>
#include <xdc/runtime/System.h>

TwoWire::TwoWire()
{
}
//Purpose is to get i2c initialized
void TwoWire::begin(){
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
//    i2cParams.transferMode = I2C_MODE_BLOCKING; //We want it to block on a semaphore
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
    if(I2C_transfer(i2c, &i2cTransaction)){
        System_printf("Transfer Success\n");
        System_flush();
        i2cTransaction.readCount = 0;
        txBufferLength = 0;
        txBufferIndex = 0;
        return 0;
    }
    return 1;
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
