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

//NEED THIS FOR A SEMAPHORE TO WORK!!!!!!!!!!!!!!!!
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header files */
#include "Board.h"

/*LSM9DS1 Files*/
#include "LSM9DS1/LSM9DS1.h"

#define TASKSTACKSIZE   512

//Main task
Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

//Printing Task
Task_Struct printerStruct;
Char printerStack[TASKSTACKSIZE];

//LSM Reset Task
Task_Struct lsmResetStruct;
Char lsmResetStack[TASKSTACKSIZE];

static PIN_State rstPinState;
static PIN_Handle rstPinHandle;

PIN_Config rstPinTable[] = {
    Board_LSM0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

LSM9DS1         imu;

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */

//This is the function that will trigger the reset of the LSM9DS1
void resetLSM(){
    System_printf("Resetting\n");
    System_flush();
    Semaphore_post(lsmSem);
    Semaphore_pend(lsmWaitSem, BIOS_WAIT_FOREVER);
    System_printf("Yo, we finished the lsmwaitsem\n");
    System_flush();
}

//This is just a function that grabs values from the IMU
Void heartBeatFxn(UArg arg0, UArg arg1)
{
    LSM9DS1Init(&imu);
    while(!LSM9DS1begin(&resetLSM, &imu)){
        System_printf("This shouldn't happen\n");
        System_flush();
    }

    while(1){
        LSM9DS1readAccel(&imu);
        LSM9DS1readGyro(&imu);
        Semaphore_post(printSem); //Printing out the values gathered from the IMU
    }
}

//Actually resets the LSM9DS1
Void resetAccel(){
    for(;;){
        Semaphore_pend(lsmSem, BIOS_WAIT_FOREVER);
        System_printf("We're in the resetAccel function now\n");
        System_flush();
        PIN_setOutputValue(rstPinHandle, Board_LSM0, 0);
        Task_sleep(10000);
        PIN_setOutputValue(rstPinHandle, Board_LSM0, 1);
        Semaphore_post(lsmWaitSem);
    }
}

//Needs to be reworked
Void printData(){
    for(;;){
        Semaphore_pend(printSem, BIOS_WAIT_FOREVER);
        System_printf("Accel %f %f %f \n", LSM9DS1calcAccel(imu.ax, &imu), LSM9DS1calcAccel(imu.ay, &imu), LSM9DS1calcAccel(imu.az, &imu));
        System_printf("Gyro %f %f %f \n", LSM9DS1calcGyro(imu.gx, &imu), LSM9DS1calcGyro(imu.gy, &imu), LSM9DS1calcGyro(imu.gz,&imu));
        System_flush();
    }
}

/*
 *  ======== main ========
 */

#define LSM9DS1_M   0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

int main(void)
{
    //Task parameter objects
    Task_Params dataGather;
    Task_Params printerParams;
    Task_Params lsmResetParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&dataGather);
    dataGather.stackSize = TASKSTACKSIZE;
    dataGather.stack = &task0Stack;
    dataGather.priority = 2;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &dataGather, NULL);

    //This should be pending on a semaphore
    Task_Params_init(&printerParams);
    printerParams.stackSize = TASKSTACKSIZE;
    printerParams.stack = &printerStack;
    printerParams.priority = 3;
    Task_construct(&printerStruct, (Task_FuncPtr)printData, &printerParams, NULL);

    //LSM Reset Task Setup
    Task_Params_init(&lsmResetParams);
    lsmResetParams.stackSize = TASKSTACKSIZE;
    lsmResetParams.stack = &lsmResetStack;
    lsmResetParams.priority = 1;
    Task_construct(&lsmResetStruct, (Task_FuncPtr)resetAccel, &lsmResetParams, NULL);

    //Setting up the rst pin for the LSM9DS1
    rstPinHandle = PIN_open(&rstPinState, rstPinTable);
    if(!rstPinHandle) {
        System_abort("Error initializing board LED pins\n");
    }
    //Setting the initial pin high
    PIN_setOutputValue(rstPinHandle, Board_LSM0, 1);


    //Setting up IMU variables
    imu.settings.device.commInterface = IMU_MODE_I2C;
    imu.settings.device.mAddress = LSM9DS1_M;
    imu.settings.device.agAddress = LSM9DS1_AG;


//    System_printf("Starting the example\nSystem provider is set to SysMin. "
//                  "Halt the target to view any SysMin contents in ROV.\n");
//    /* SysMin will only print to the console when you call flush or exit */
//
//    System_flush();
    BIOS_start();

    return (0);
}
