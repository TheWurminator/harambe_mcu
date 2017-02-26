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
#include "../Board.h"
//Initialization function for the STN
uint8_t STN1110::begin(){

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
       return 1;
    }

    char data[20];
    runCommand("AT E0", data, 20);
    return runCommand("AT SP 0", data, 20);
}

uint8_t STN1110::engineLoad(uint8_t &load){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","04",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    load=values[0]*100/255;
    return ELM_SUCCESS;
}

uint8_t STN1110::engineRPM(int &rpm){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","0C",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    rpm=((values[0]*256)+values[1])/4;
    return ELM_SUCCESS;
}

uint8_t STN1110::vehicleSpeed(uint8_t &speed){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","0D",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    speed=values[0];
    return ELM_SUCCESS;
}


uint8_t STN1110::throttlePosition(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","11",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(values[0]*100)/255;
    return ELM_SUCCESS;
}


uint8_t STN1110::auxiliaryInputStatus(bool &auxStatus){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","1E",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    auxStatus=getBit(values[0], 1);
    return ELM_SUCCESS;
}

uint8_t STN1110::engineRunTime(unsigned int &runTime){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","1F",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    runTime=(values[0]*256)+values[1];
    return ELM_SUCCESS;
}

uint8_t STN1110::distanceMIL(unsigned int &distance){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","21",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    distance=(values[0]*256)+values[1];
    return ELM_SUCCESS;
}

uint8_t STN1110::commandedEGR(uint8_t &egr){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","2C",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    egr=(100 * values[0])/255;
    return ELM_SUCCESS;
}

uint8_t STN1110::EGRError(int &error){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","2D",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    error =(values[0]-128)*100/128;
    return ELM_SUCCESS;
}

uint8_t STN1110::warmUpsSinceLastCleared(uint8_t &warmUps){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","30",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    warmUps=values[0];
    return ELM_SUCCESS;
}

uint8_t STN1110::distanceSinceLastCleared(unsigned int &distance){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","31",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    distance=(values[0]*256) + values[1];
    return ELM_SUCCESS;
}

uint8_t STN1110::controlModuleVoltage(unsigned int &voltage){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","42",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    voltage=((values[0]*256)+values[1])/1000;
    return ELM_SUCCESS;
}

uint8_t STN1110::absoluteLoadValue(unsigned int &load){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","43",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    load=((values[0]*256)+values[1])*100/255;
    return ELM_SUCCESS;
}

uint8_t STN1110::commandEquivalenceRatio(float &ratio){
    uint8_t status;
    uint8_t values[2];
    status=getuint8_ts("01","41","44",values,2);
    if (status != ELM_SUCCESS){
        return status;
    }
    ratio=((values[0]*256)+values[1])/32768;
    return ELM_SUCCESS;
}


uint8_t STN1110::relativeThrottlePosition(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","45",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}

uint8_t STN1110::absoluteThrottlePositionB(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","47",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}
uint8_t STN1110::absoluteThrottlePositionC(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","48",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}
uint8_t STN1110::acceleratorPedalPositionD(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","49",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}
uint8_t STN1110::acceleratorPedalPositionE(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","4A",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}
uint8_t STN1110::acceleratorPedalPositionF(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","4B",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}
uint8_t STN1110::commandedThrottleActuator(uint8_t &position){
    uint8_t status;
    uint8_t values[1];
    status=getuint8_ts("01","41","4C",values,1);
    if (status != ELM_SUCCESS){
        return status;
    }
    position=(100*values[0])/255;
    return ELM_SUCCESS;
}

uint8_t STN1110::getuint8_ts( const char *mode, const char *chkMode, const char *pid, uint8_t *values, unsigned int numValues){
    char data[64];
    uint8_t status;
    char hexVal[]="0x00";
    char cmd[6];
    cmd[0]=mode[0];
    cmd[1]=mode[1];
    cmd[2]=' ';
    cmd[3]=pid[0];
    cmd[4]=pid[1];
    cmd[5]='\0';

    status=runCommand(cmd,data,64);
    if ( status != ELM_SUCCESS )
    {
        return status;
    };

    // Check the mode returned was the one we sent
    if ( data[0]!=chkMode[0]
      or data[1]!=chkMode[1]
      or data[3]!=pid[0]
      or data[4]!=pid[1] ){
        return ELM_GARBAGE;
    }

    // For each uint8_t expected, package it up
    int i=0;
    for (i=0; i<numValues; i++){
        hexVal[2]=data[6+(3*i)];
        hexVal[3]=data[7+(3*i)];
        values[i]=strtol(hexVal,NULL,16);
    }
    return ELM_SUCCESS;
}



//This is the main thing that we need to dfix
uint8_t STN1110::runCommand(const char *cmd, char *data, unsigned int dataLength)
{
    // Flush any leftover data from thes last command.

    // Send the specified command to the controller.
    flush();
    char rt = '\r';
    UART_write(uart, cmd, strlen(cmd));
    UART_write(uart, (const void *)&rt, 1);

    int counter;
    bool found;

    // Start reading the data right away and don't stop
    // until either the requested number of uint8_ts has
    // been read or the timeout is reached, or the >
    // has been returned.
    //
    counter=0;
    found=false;
    const char * randomString = "XXXXXXXXX\r\r\r";
    uint8_t rsSize = strlen(randomString);
    //Remember that there was a timeout here
    //This is just reading until there is a command line
    while (!found && counter<( dataLength ))
    {
        UART_read(uart, &data[counter], 1);
        if (data[counter] == '>' ){
            found=true;
            data[counter]='\0';
        }else{
            ++counter;
        }
    }
    // If there is still data pending to be read, raise OVERFLOW error.
    if (!found  && counter>=dataLength)
    {
        // Send a character, this should cancel any operation on the elm device
        // so that it doesnt spuriously inject a response during the next
        // command
//        ELM_PORT.print("XXXXXXXXX\r\r\r");
        UART_write(uart, randomString, rsSize);
//        delay(300);
        return ELM_BUFFER_OVERFLOW;
    }

    // If not found, and there is still buffer space, then raise no response error.
    if (!found && counter<dataLength){
        // Send a character, this should cancel any operation on the elm device
        // so that it doesnt spuriously inject a response during the next
        // command
//        ELM_PORT.print("XXXXXXXXX\r\r\r");
        UART_write(uart, randomString, rsSize);
//        delay(300);
        return ELM_NO_RESPONSE;
    }

    char *match;
    match=strstr(data,"UNABLE TO CONNECT");
    if (match != NULL){
        return ELM_UNABLE_TO_CONNECT;
    }
    match=strstr(data,"NO DATA");
    if (match != NULL){
        return ELM_NO_DATA;
    }
    if (strncmp(data,"SEARCHING...",12)==0)
    {
        // Remove searching...
        uint8_t i=12;
        while (data[i]!='\0'){
            data[i-12]=data[i];
            i++;
        }
        data[i]='\0';
    }

    // Otherwise return success.
    return ELM_SUCCESS;
}

uint8_t STN1110::getVoltage(float &voltage){
    char data[20];
    uint8_t status;
    char cmd[]="ATRV";
    status=runCommand(cmd,data,20);
    if (status==ELM_SUCCESS){
        voltage=atof(data);
    }
    return status;
}


//Flush out the read buffer
//Just read out until it's zeroes
void STN1110::flush(){
    char x = 1;
    UART_read(uart, &x, 1);
    while (x){
        UART_read(uart, &x, 1);
    }
}

/** returns the value of the specified bit p in uint8_t b
 */
bool STN1110::getBit(uint8_t b, uint8_t p)
{
    b<<=(7-p);
    if (b>=127){
        return true;
    }
    return false;
}
