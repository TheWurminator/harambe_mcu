#include "STN1110.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ti/drivers/UART.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>
#include "../Board/Board.h"


//Initialization function for the STN
char resetCommand[] = "ATZ";
char noEcho[] = "ATE0";
char vehicleSpeed[] = "010D";
char throttlePosition[] = "0111";
uint8_t newline = 0xd;
uint8_t thing[17];

uint8_t init(STN1110 * val){
    UART_Params uartParams = val->uartParams;
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.baudRate = 9600;
    uartParams.readTimeout = Clock_tickPeriod * 10000;
    val->uart = UART_open(Board_UART0, &uartParams);
    if (val->uart == NULL) {
       System_printf("UART initialization failed\n");
       System_flush();
       return 1;
    }
    return 0;
}
uint8_t elmCheck(STN1110 *val){
    System_printf("Inside of elmcheck\n");
    System_flush();
    char x = 0;
    int chkflg = 0;
    //This will send a command to reset the STN1110
    UART_write(val->uart, &resetCommand, 3);
    UART_write(val->uart, &newline, 1);
    System_printf("5 minutes\n");
    System_flush();
    while(UART_read(val->uart, &x, 1)){
        if(x == 'E' || x == 'L' || x == 'M'){
            chkflg++;
        }
    }
    if(chkflg != 3){
        System_printf("Unsuccessful\n");
        System_flush();
        return 0;
    }
    else{
        return 1;
    }
}

uint8_t begin(STN1110 *val){
    //Check to see if the ELM327 is present
   while(!elmCheck(val)){

   }
   //Turning off echo
   UART_write(val->uart, &noEcho, 4);
   UART_write(val->uart, &newline, 1);
   char x = 0;
   uint8_t checkflg = 0;
   while(UART_read(val->uart, &x, 1)){
       if(x == 'K') checkflg++;
   }
   if (checkflg) return ELM_SUCCESS;
   else return 1;
}

uint8_t getSpeed(STN1110* val){
   UART_write(val->uart, &vehicleSpeed,4);
   UART_write(val->uart, &newline, 1);
   char x = 0;
   uint8_t flag0 = 0;
   while(UART_read(val->uart, &x, 1)){
       //Searching for the 0D in the stream
       if (x == '0' && !flag0){
           flag0 = 1;
       }
       else if(x == 'D' && flag0){
           //System_printf("Got to this point\n");
           System_flush();
           UART_read(val->uart, &x, 1); //Extra space
           uint8_t valu = 0;
           uint8_t i = 0;
           //Calculating the actual speed from the values
           for(i = 0; i < 2; i++){
               UART_read(val->uart, &x, 1);
               if(i == 0){
                   valu += transform(x) << 4;
               }
               else{
                   valu |= transform(x);
               }
           }
           return valu; //Returning the speed
       }
       else{
           flag0 = 0;
       }
       System_flush();
   }
   System_flush();
   return 0;
}


uint8_t transform(char x){
    if (x >= 'A' && x <= 'F'){
        return x - 55;
    }
    else{
        return x - 48;
    }
}

