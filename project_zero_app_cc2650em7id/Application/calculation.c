#include <string.h>

//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <xdc/std.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

// Stack headers
#include <hci_tl.h>
#include <osal_snv.h>
#include <peripheral.h>
#include <xdc/runtime/System.h>
#include <devinfoservice.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/cfg/global.h>
#include "util.h"
#include <ti/sysbios/BIOS.h>
#include "Board.h"

#include "calculation.h"

typedef struct MsgObj {
    Int id;             /* writer task id */
    Int val;            /* message value */
} MsgObj, *Msg;
#define PRZ_TASK_PRIORITY2                     1

#ifndef PRZ_TASK_STACK_SIZE
#define PRZ_TASK_STACK_SIZE2                  800
#endif
extern const ti_sysbios_knl_Mailbox_Handle mbx;
//static Queue_Struct applicationMsgQ2;
//static Queue_Handle hApplicationMsgQ2;

//static Mailbox_Handle hMyCoolMailBox;
//static Mailbox_Struct myCoolMailBox;

// Task configuration
Task_Struct przTask2;
char przTaskStack2[PRZ_TASK_STACK_SIZE2];


/*
 *  Using default Mailbox_Params...
 *
 *
typedef struct Mailbox_Params {

// Instance config-params structure
    IInstance_Params *instance;
    // Common per-instance configs
    Ptr buf;
    // The address of the buffer used for creating messages
    UInt bufSize;
    // The size of the buffer that 'buf' points to
    IHeap_Handle heap;
    // The IHeap instance used for dynamic creates
    Event_Handle readerEvent;
    // Mailbox not empty event if using Events. Default is null
    UInt readerEventId;
    // Mailbox not empty event Id if using Events. Default is 1
    Event_Handle writerEvent;
    // Mailbox not full event if using Events. Default is null
    UInt writerEventId;
    // Mailbox not full event Id if using Events. Default is 1
} Mailbox_Params;
*/

static void calculation_init( void );
static void calculation_taskFxn(UArg a0, UArg a1);

Void Mailbox_Params_init();


//void Mailbox_Params_init(){

    //Mailbox_params.buf = null;





//}

void calculation_createTask(void)
{
  Task_Params taskParams2;

  // Configure task
  Task_Params_init(&taskParams2);
  taskParams2.stack = przTaskStack2;
  taskParams2.stackSize = PRZ_TASK_STACK_SIZE2;
  taskParams2.priority = 2;

  Task_construct(&przTask2, calculation_taskFxn, &taskParams2, NULL);
}


static void calculation_init(void)
{
  // ******************************************************************
  // NO STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages via ICall to Stack.

  System_printf("Starting calc init\n");
        System_flush();
 //Mailbox_Params_init();

 // Queue_construct(&applicationMsgQ2, NULL);
  //hApplicationMsgQ2 = Queue_handle(&applicationMsgQ2);

 // Mailbox_construct(&myCoolMailBox, 4, 1, NULL, NULL);
     //   hMyCoolMailBox = Mailbox_create(4,1,NULL, NULL);
 // hMyCoolMailBox = Mailbox_handle(&myCoolMailBox);

//typedef struct MsgObj {
  //  Int id;             /* writer task id */
 //   Int val;            /* message value */
//} MsgObj, *Msg;

}
static void calculation_taskFxn(UArg a0, UArg a1) {
   // MsgObj      msg;
  // Initialize application
   // System_printf("running0\n");
     //    System_flush();
  calculation_init();
  // mailboxSuperMSG.id = 1;
  // Application main loop
  // BIOS_WAIT_FOREVER
   MsgObj      mailboxSuperMSG;
   Int         mailboxSuperID;

  for (;;){
     System_printf("Going into main loop of calculation task\n");
     // System_flush();
      /*
       *    We will pend our mailbox here waiting for a post from another task. In this case, the I2C and UART tasks will post to this mailbox.
       *
       *
       *
       */
      int_fast16_t check = Mailbox_pend(mbx, &mailboxSuperMSG,  BIOS_WAIT_FOREVER);

      System_printf("The value of check is: %d", check);
     // System_flush();
      System_printf("read '%d' from (%d).\n", mailboxSuperMSG.val, mailboxSuperMSG.id);
        if (check==0){
                  //System_printf("\nreader: timeout expired for Mailbox_pend()\n");
                //  System_flush();
          System_printf("read '%d' from (%d).\n", mailboxSuperMSG.val, mailboxSuperMSG.id);
                System_flush();
            //    System_printf("Got past the pend!\n");
            //    System_flush();
              //   break;


    //  System_printf("read '%d' from (%d).\n", mailboxSuperMSG.val, mailboxSuperMSG.id);
   //   System_flush();
    //  System_printf("Got past the pend!\n");
    //  System_flush();
     // break;
        }
      else if (check==1){
      //    System_printf("\n reader: timeout expired for Mailbox_pend() \n");
      //    System_flush();
      }


        Task_yield();
  }

}
