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
#include "LSM9DS1/Wire.h"

#define TASKSTACKSIZE   512

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    PIN_TERMINATE
};

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void heartBeatFxn(UArg arg0, UArg arg1)
{
//    I2C_Handle      i2c;
//    I2C_Params      i2cParams;
//    I2C_Transaction i2cTransaction;
//    uint8_t         txBuffer[1];
//    uint8_t         rxBuffer[2];
//    int             temperature;
//    int             i;

    LSM9DS1 thing;
    thing.begin();

    /* Point to the T ambient register and read its 2 bytes */
//    txBuffer[0] = TMP007_OBJ_TEMP;
//    i2cTransaction.slaveAddress = Board_TMP007_ADDR;
//    i2cTransaction.writeBuf = txBuffer;
//    i2cTransaction.writeCount = 1;
//    i2cTransaction.readBuf = rxBuffer;
//    i2cTransaction.readCount = 2;
//
//    /* Take 20 samples and print them out onto the console */
//    for (i = 0; i < 20; i++) {
//        if (I2C_transfer(i2c, &i2cTransaction)) {
//            /* Extract degrees C from the received data; see TMP102 datasheet */
//            temperature = (rxBuffer[0] << 6) | (rxBuffer[1] >> 2);
//
//            /*
//             * If the MSB is set '1', then we have a 2's complement
//             * negative value which needs to be sign extended
//             */
//            if (rxBuffer[0] & 0x80) {
//                temperature |= 0xF000;
//            }
//           /*
//            * For simplicity, divide the temperature value by 32 to get rid of
//            * the decimal precision; see TI's TMP007 datasheet
//            */
//            temperature /= 32;
//
//            System_printf("Sample %u: %d (C)\n", i, temperature);
//        }
//        else {
//            System_printf("I2C Bus fault\n");
//        }
//
//        System_flush();
//        Task_sleep(1000000 / Clock_tickPeriod);
//    }

    /* Deinitialized I2C */
//    I2C_close(i2c);
//    System_printf("I2C closed!\n");
//    System_flush();
//    while (1) {
//        Task_sleep((UInt)arg0);
//    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();
    // Board_initSPI();
    Board_initUART();
    // Board_initWatchdog();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&taskParams);
//    taskParams.arg0 = 1000000 / Clock_tickPeriod;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
