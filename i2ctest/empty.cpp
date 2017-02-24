/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header files */
#include "Board.h"

/*LSM9DS1 Files*/
#include "LSM9DS1/SparkFunLSM9DS1.h"

#define TASKSTACKSIZE   512

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
//I2C_Handle i2c;
/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    PIN_TERMINATE
};

LSM9DS1 imu;
I2C_Handle      i2c;

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    while(!imu.begin(&i2c)){
        System_printf("FUCK \n");
        System_flush();
    }
//      Wire.begin();
//
//      int i = 0;
//      for(i;i<5;i++){
//          Wire.beginTransmission(0x1e);
//          Wire.write(WHO_AM_I_M);
//          Wire.endTransmission();
//          //Now its time to read
//          System_printf("0x%x\n",Wire.read());
//      }

//    I2C_Params i2cParams;
//    I2C_Transaction i2cTransaction;
//    uint8_t rxBuffer[1];
//    uint8_t txBuffer[1];
//    I2C_Params_init(&i2cParams);
//    i2cParams.bitRate = I2C_400kHz;
//    i2c = I2C_open(Board_I2C0, &i2cParams);
//    if (i2c == NULL) {
//        System_abort("Error Initializing I2C\n");
//        System_flush();
//    }
//    else {
//        System_printf("I2C Initialized!\n");
//        System_flush();
//    }
//    int i = 0;
//    while(i<4){
//        //Reading from the magnetometer
//        i2cTransaction.slaveAddress = 0x1e;
//        i2cTransaction.readBuf = rxBuffer;
//        i2cTransaction.readCount = 1;
//        i2cTransaction.writeCount = 1;
//        i2cTransaction.writeBuf = txBuffer;
//
//        txBuffer[0] = WHO_AM_I_M;
//        if(I2C_transfer(i2c, &i2cTransaction)){
//            System_printf("At least this is working\n");
//            System_flush();
//            uint8_t name = rxBuffer[0];
//            System_printf("0x%x\n", name);
//            System_flush();
//        }
//        else{
//            System_printf("i2c bus fault \n");
//            System_flush();
//        }
//        i++;
//    }
}

/*
 *  ======== main ========
 */

#define LSM9DS1_M   0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
//    taskParams.arg0 = 1000000 / Clock_tickPeriod;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
    imu.settings.device.commInterface = IMU_MODE_I2C;
    imu.settings.device.mAddress = LSM9DS1_M;
    imu.settings.device.agAddress = LSM9DS1_AG;


    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
    BIOS_start();

    return (0);
}
