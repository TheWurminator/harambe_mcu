#include <string.h>

//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/drivers/PIN.h>
#include <ti/mw/display/Display.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

// Stack headers
#include <hci_tl.h>
#include <gap.h>
#include <gatt.h>
#include <gapgattserver.h>
#include <gattservapp.h>
#include <osal_snv.h>
#include <gapbondmgr.h>
#include <peripheral.h>
#include <icall_apimsg.h>
#include <xdc/runtime/System.h>
#include <devinfoservice.h>

#include "util.h"

#include "Board.h"
#include "calculation.h"

// Bluetooth Developer Studio services
#include "led_service.h"
#include "button_service.h"
#include "data_service.h"

#define PRZ_TASK_PRIORITY2                     1

#ifndef PRZ_TASK_STACK_SIZE
#define PRZ_TASK_STACK_SIZE2                  800
#endif

static ICall_Semaphore sem2;
static ICall_EntityID selfEntity2;

static Queue_Struct applicationMsgQ2;
static Queue_Handle hApplicationMsgQ2;

// Task configuration
Task_Struct przTask2;
char przTaskStack2[PRZ_TASK_STACK_SIZE2];


static void calculation_init( void );
static void calculation_taskFxn(UArg a0, UArg a1);




void calculation_createTask(void)
{
  Task_Params taskParams2;

  // Configure task
  Task_Params_init(&taskParams2);
  taskParams2.stack = przTaskStack2;
  taskParams2.stackSize = PRZ_TASK_STACK_SIZE2;
  taskParams2.priority = 1;

  Task_construct(&przTask2, calculation_taskFxn, &taskParams2, NULL);
}


static void calculation_init(void)
{
  // ******************************************************************
  // NO STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages via ICall to Stack.
  ICall_registerApp(&selfEntity2, &sem2);

  Log_info0("Initializing the user task, hardware, BLE stack and services.");

  // Open display. By default this is disabled via the predefined symbol Display_DISABLE_ALL.
 // dispHandle = Display_open(Display_Type_LCD, NULL);

  // Initialize queue for application messages.
  // Note: Used to transfer control to application thread from e.g. interrupts.
  Queue_construct(&applicationMsgQ2, NULL);
  hApplicationMsgQ2 = Queue_handle(&applicationMsgQ2);

}

static void calculation_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
   // System_printf("running0\n");
     //    System_flush();
  calculation_init();

  // Application main loop
  for (;;){

    //  System_printf("New Function\n");
   //   System_flush();
      ICall_Errno errno2 = ICall_wait(ICALL_TIMEOUT_FOREVER);
      //if (errno2 == 0){
   //   ICall_Errno errno2 = ICall_wait(ICALL_TIMEOUT_FOREVER);
     //     if (errno2 == ICALL_ERRNO_SUCCESS)
       //   {
         // }
Task_sleep(1000);
}


  }

