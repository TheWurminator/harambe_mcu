/* Copyright 2011 David Irvine
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/



#include "STN1110.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ti/drivers/UART.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>

#include "../Board.h"
//Initialization function for the STN
char resetCommand[] = "ATZ";
char noEcho[] = "ATE0";
char vehicleSpeed[] = "010D";
char throttlePosition[] = "0111";
uint8_t newline = 0xd;
uint8_t thing[17];
uint8_t STN1110::begin(){
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uartParams.readTimeout = Clock_tickPeriod * 10000;
    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
       System_printf("UART initialization failed\n");
       System_flush();
       return 1;
    }
    //Check to see if the ELM327 is present
    if(elmCheck()){
        //Turning off echo
        UART_write(uart, &noEcho, 4);
        UART_write(uart, &newline, 1);
        char x = 0;
        uint8_t checkflg = 0;
        while(UART_read(uart, &x, 1)){
            if(x == 'K') checkflg++;
        }
        if (checkflg) return ELM_SUCCESS;
        return 1;
    }
    else return 1;
}

uint8_t STN1110::elmCheck(){
    System_printf("Inside of elmcheck\n");
    System_flush();
    char x = 0;
    int chkflg = 0;
    //This will send a command to reset the STN1110
    UART_write(uart, &resetCommand, 3);
    UART_write(uart, &newline, 1);
    while(UART_read(uart, &x, 1)){
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

uint8_t STN1110::engineRPM(int &rpm){
    return 0;
}

uint8_t STN1110::getSpeed(){
    UART_write(uart, &vehicleSpeed,4);
    UART_write(uart, &newline, 1);
    char x = 0;
    while(UART_read(uart,&x,1)){
        System_printf("%c", x);
    }
    System_flush();
    return 1;
}

uint8_t STN1110::getThrottlePosition(){
    UART_write(uart, &throttlePosition, 4);
    UART_write(uart, &newline, 1);
    char x = 0;
    while(UART_read(uart,&x, 1)){
        System_printf("%c", x);
    }
    System_flush();
    return 1;
}

