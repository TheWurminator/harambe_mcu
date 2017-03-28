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
//Gross
//typedef ti_sysbios_knl_Mailbox_Object* ti_sysbios_knl_Mailbox_Handle
extern const ti_sysbios_knl_Mailbox_Handle mbx;

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <math.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header files */
#include "Board.h"

/*LSM9DS1 Files*/
#include "LSM9DS1/LSM9DS1.h"
#include "stn/STN1110.h"

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

Task_Struct messejiStruct;
Char messejiStack[TASKSTACKSIZE];

Task_Struct stnStruct;
Char stnStack[TASKSTACKSIZE];

static PIN_State rstPinState;
static PIN_Handle rstPinHandle;

PIN_Config rstPinTable[] = {
    Board_LSM0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

LSM9DS1         imu;

typedef struct{
    uint8_t uid;
    float deltaAccel;
    float deltaGyro;
}messeji;



/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */

//This is the function that will trigger the reset of the LSM9DS1
void resetLSM(){
    Semaphore_pend(carSem, BIOS_WAIT_FOREVER);

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
    Semaphore_pend(carSem, BIOS_WAIT_FOREVER);

    LSM9DS1Init(&imu);
    while(!LSM9DS1begin(&resetLSM, &imu)){
        System_printf("This shouldn't happen\n");
        System_flush();
    }

    while(1){
        LSM9DS1readAccel(&imu);
        LSM9DS1readGyro(&imu);
        Semaphore_post(printSem); //Printing out the values gathered from the IMU
        Semaphore_pend(postWaitSem, BIOS_WAIT_FOREVER);
    }
}

//Actually resets the LSM9DS1
Void resetAccel(){
    Semaphore_pend(carSem, BIOS_WAIT_FOREVER);

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
    Semaphore_pend(carSem, BIOS_WAIT_FOREVER);
    float accel[5][3];
    float gyro[5][3];
    int i = 0;
    for(;;){
        Semaphore_pend(printSem, BIOS_WAIT_FOREVER);
        if(i < 5){
//            System_printf("Accel %f %f %f \n", LSM9DS1calcAccel(imu.ax, &imu), LSM9DS1calcAccel(imu.ay, &imu), LSM9DS1calcAccel(imu.az, &imu));
//            System_printf("Gyro %f %f %f \n", LSM9DS1calcGyro(imu.gx, &imu), LSM9DS1calcGyro(imu.gy, &imu), LSM9DS1calcGyro(imu.gz,&imu));
            accel[i][0] = LSM9DS1calcAccel(imu.ax, &imu);
            accel[i][1] = LSM9DS1calcAccel(imu.ay, &imu);
            accel[i][2] = LSM9DS1calcAccel(imu.az, &imu);
            gyro[i][0] = LSM9DS1calcGyro(imu.gx, &imu);
            gyro[i][1] = LSM9DS1calcGyro(imu.gy, &imu);
            gyro[i][2] = LSM9DS1calcGyro(imu.gz, &imu);
            i++;
        }
        else{
            double deltaAccel = 0.0;
            double deltaGyro = 0.0;
            for(i=0;i<5;i++){
                deltaAccel += sqrt(pow((double)accel[i][0],2.0) + pow((double)accel[i][2],2.0) + pow((double)accel[i][1],2.0));
                deltaGyro += sqrt(pow((double)gyro[i][0],2.0) + pow((double)gyro[i][1],2.0) + pow((double)gyro[i][2],2.0));
            }
            i=0;
            deltaAccel = deltaAccel / 5.0;
            deltaGyro = deltaGyro / 5.0;
            messeji frank;
            frank.uid = 1;
            frank.deltaAccel = deltaAccel;
            frank.deltaGyro = deltaGyro;
            int ret = Mailbox_post(mbx, &frank, BIOS_WAIT_FOREVER);
        }
        Semaphore_post(postWaitSem);
    }
}

STN1110 lala;

Void stnFxn()
{
    int x = 0;
    label:
    x = x+1;
    int thing = init(&lala);
    if(!thing){
        System_printf("The initialization of the chip was successful\n");
        System_flush();
        //Need to perform the check once, definitely don't need to do it every time we want data
        volatile int ret = begin(&lala);
        while(1){
            while(ret != ELM_SUCCESS){ //Waiting for everything to be initialized properly
                System_printf("Unsuccessful Start\n");
                System_flush();
                ret = begin(&lala);
            }
            //Getting speed of the vehicle
            uint8_t speeds[5];
            uint8_t i = 0;
            for(i = 0; i < 5; i++){
                uint8_t ret = getSpeed(&lala);
                if(ret < 256){
                    speeds[i] = getSpeed(&lala);
                }
                else{
                    i--;
                }
            }
            System_printf("%d, %d, %d, %d, %d\n", speeds[0], speeds[1], speeds[2], speeds[3], speeds[4]);
            System_flush();
            //Really need to collect the speed 5 times
        }
    }
    else{
        System_printf("UART NOT INITIALIZED PROPERLY\n");
        System_flush();
        goto label;
    }

}

Void calculationFxn(){
    Semaphore_pend(carSem, BIOS_WAIT_FOREVER);
    messeji supermesseji;
    messeji supermesseji2;
    for(;;){
        int_fast16_t ret = Mailbox_pend(mbx, &supermesseji, BIOS_WAIT_FOREVER);
        if (ret){
            if(deltaAccel > 2.0 || deltaGyro > 2.0){
                //Do bluetooth
            }
            System_printf("haha it's working\n");
            System_printf("The value for deltaaccel is %f\n", supermesseji.deltaAccel);
            System_printf("The value for deltagyro is %f\n", supermesseji.deltaGyro);
            System_flush();
        }
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
    Task_Params messejiParams;
    Task_Params stnParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();
    Board_initUART();

    /* Construct heartBeat Task  thread */
    Task_Params_init(&dataGather);
    dataGather.stackSize = TASKSTACKSIZE;
    dataGather.stack = &task0Stack;
    dataGather.priority = 1;
    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &dataGather, NULL);

    //This should be pending on a semaphore
    Task_Params_init(&printerParams);
    printerParams.stackSize = TASKSTACKSIZE;
    printerParams.stack = &printerStack;
    printerParams.priority = 1;
    Task_construct(&printerStruct, (Task_FuncPtr)printData, &printerParams, NULL);

    //LSM Reset Task Setup
    Task_Params_init(&lsmResetParams);
    lsmResetParams.stackSize = TASKSTACKSIZE;
    lsmResetParams.stack = &lsmResetStack;
    lsmResetParams.priority = 1;
    Task_construct(&lsmResetStruct, (Task_FuncPtr)resetAccel, &lsmResetParams, NULL);

    Task_Params_init(&messejiParams);
    messejiParams.stackSize = TASKSTACKSIZE;
    messejiParams.stack = &messejiStack;
    messejiParams.priority = 2;
    Task_construct(&messejiStruct, (Task_FuncPtr)calculationFxn, &messejiParams, NULL);

    //Car Task
    Task_Params_init(&stnParams);
    stnParams.stackSize = TASKSTACKSIZE;
    stnParams.stack = &stnStack;
    stnParams.priority = 3;
    Task_construct(&stnStruct, (Task_FuncPtr)stnFxn, &stnParams, NULL);

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
    BIOS_start();

    return (0);
}
