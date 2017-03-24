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
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"
#include "stn/STN1110.h"

#include <stdint.h>

#define TASKSTACKSIZE     1024

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

STN1110 lala;
/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */

Void echoFxn(UArg arg0, UArg arg1)
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
/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initUART();

    /* Construct BIOS objects */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.priority = 2;
    Task_construct(&task0Struct, (Task_FuncPtr)echoFxn, &taskParams, NULL);

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
