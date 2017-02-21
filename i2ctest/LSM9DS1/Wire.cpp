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
    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
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
    //set address of targeted slave
    txaddress = address;
    //Indicate that we're transmitting
    transmitting = 1;
    // reset tx buf iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;
}

//Send the TX Buffer, bust send restart to keep connection alive
uint8_t TwoWire::endTransmission(bool tf){
    //Send i2c
    i2cTransaction.slaveAddress = txbuffer[0];
    i2cTransaction.writeBuf = txbuffer;
    i2cTransaction.writeCount = txBufferLength;
    //Actually sending the byte
    I2C_transfer(i2c, &i2cTransaction);
    //Reset all of the variables
    i2cTransaction.slaveAddress = 0;
    i2cTransaction.writeBuf = 0;
    i2cTransaction.writeCount = 0;
    txBufferIndex = 0;
    txBufferLength = 0;
    // indicate that we are done transmitting
    transmitting = 0;
    return 0;
}

//Used to fill up the tx buffer byte by byte
uint8_t TwoWire::write(uint8_t data){
    if(transmitting){
        //In master transmitter mode
        if(txBufferLength >= BUFFER_LENGTH){
            System_printf("What in the fuck");
            System_flush();
            return 1;
        }
        //Put byte in tx buffer
        txbuffer[txBufferIndex] = data;
        ++txBufferIndex;
        //Update amount in buffer
        txBufferLength = txBufferIndex;
    }
    return 0; //Everything worked as planned
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
  // clamp to buffer length
  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }
  // perform blocking read into buffer

  //uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);
  i2cTransaction.slaveAddress = address;
  i2cTransaction.readBuf = rxbuffer;
  i2cTransaction.readCount = quantity;
  I2C_transfer(i2c, &i2cTransaction);
  // set rx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = quantity;
  return 0;
}

uint8_t TwoWire::read(){
    uint8_t value = 0;

    //get each successive byte on each call
    if(rxBufferIndex < rxBufferLength){
        value = rxbuffer[rxBufferIndex];
        ++rxBufferIndex;
    }

    return value;
}

TwoWire Wire = TwoWire();
