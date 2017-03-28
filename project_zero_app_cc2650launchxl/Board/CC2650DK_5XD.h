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
/** ============================================================================
 *  @file       CC2650DK_5XD.h
 *
 *  @brief      CC2650EM_5XD Board Specific header file.
 *              The project options should point to this file if this is the
 *              CC2650EM you are developing code for.
 *
 *  The CC2650 header file should be included in an application as follows:
 *  @code
 *  #include <Board.h>
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC2650EM_5XD_H__
#define __CC2650EM_5XD_H__

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 *  Symbol by generic Board.c to include the correct kit specific Board.c
 *  ==========================================================================*/
#define CC2650EM_5XD
#define CC2650DK_5XD

/** ============================================================================
 *  Includes
 *  ==========================================================================*/
#include <ti/drivers/PIN.h>
#include <driverlib/ioc.h>

/** ============================================================================
 *  Externs
 *  ==========================================================================*/
extern const PIN_Config BoardGpioInitTable[];

/** ============================================================================
 *  Defines
 *  ==========================================================================*/

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                <pin mapping>
 */

/* i2c pins */
#define Board_I2C0_SDA0                     IOID_2
#define Board_I2C0_SCL0                     IOID_3

/* UART Pins */
#define Board_UART_RX                       IOID_1          /* P1.7  */
#define Board_UART_TX                       IOID_0          /* P1.9  */
#define Board_UART_CTS                      PIN_UNASSIGNED
#define Board_UART_RTS                      PIN_UNASSIGNED

//Pin to reset the LSM9DS1
#define Board_LSM                           IOID_4

/* Power Board */
#define Board_3V3_EN                        IOID_14         /* P1.15 */

/** ============================================================================
 *  Instance identifiers
 *  ==========================================================================*/
/* Generic UART instance identifiers */
#define Board_UART                  CC2650DK_5XD_UART0
/* Generic Crypto instance identifiers */
#define Board_CRYPTO                CC2650DK_5XD_CRYPTO0
/* Generic I2C instance identifiers */
#define Board_I2C                   CC2650DK_5XD_I2C0

/** ============================================================================
 *  Number of peripherals and their names
 *  ==========================================================================*/

/*!
 *  @def    CC2650DK_5XD_CryptoName
 *  @brief  Enum of Crypto names on the CC2650 dev board
 */
typedef enum CC2650DK_5XD_CryptoName {
    CC2650DK_5XD_CRYPTO0 = 0,
    CC2650DK_5XD_CRYPTOCOUNT
} CC2650DK_5XD_CryptoName;

/*!
 *  @def    CC2650DK_5XD_CryptoName
 *  @brief  Enum of I2C names on the CC2650 dev board
 */
typedef enum CC2650DK_5XD_I2CName {
    CC2650DK_5XD_I2C0 = 0,
    CC2650DK_5XD_I2CCOUNT
} CC2650DK_5XD_I2CName;

/*!
 *  @def    CC2650DK_5XD_UARTName
 *  @brief  Enum of UARTs on the CC2650 dev board
 */
typedef enum CC2650DK_5XD_UARTName {
    CC2650DK_5XD_UART0 = 0,
    CC2650DK_5XD_UARTCOUNT
} CC2650DK_5XD_UARTName;

/*!
 *  @def    CC2650DK_5XD_WatchdogName
 *  @brief  Enum of Watchdogs on the CC2650DK_5XD dev board
 */
typedef enum CC2650DK_5XD_WatchdogName {
    CC2650DK_5XD_WATCHDOG0 = 0,

    CC2650DK_5XD_WATCHDOGCOUNT
} CC2650DK_5XD_WatchdogName;


/*!
 *  @def    CC2650DK_7ID_TRNGName
 *  @brief  Enum of TRNG names on the CC2650 dev board
 */
typedef enum CC2650DK_7ID_TRNGName {
    CC2650DK_7ID_TRNG0 = 0,
    CC2650DK_7ID_TRNGCOUNT
} CC2650DK_7ID_TRNGName;


#ifdef __cplusplus
}
#endif

#endif /* __CC2650EM_H__ */
