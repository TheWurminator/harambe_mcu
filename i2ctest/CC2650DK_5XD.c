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
 *  ====================== CC2650DK_5XD.c =============================================
 *  This file is responsible for setting up the board specific items for the
 *  SRF06EB with the CC2650EM_5XD board.
 */


/*
 *  ====================== Includes ============================================
 */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerCC26XX.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/ioc.h>
#include <driverlib/udma.h>

#include <Board.h>

/*
 *  ========================= IO driver initialization =========================
 *  From main, PIN_init(BoardGpioInitTable) should be called to setup safe
 *  settings for this board.
 *  When a pin is allocated and then de-allocated, it will revert to the state
 *  configured in this table.
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(BoardGpioInitTable, ".const:BoardGpioInitTable")
#pragma DATA_SECTION(PINCC26XX_hwAttrs, ".const:PINCC26XX_hwAttrs")
#endif

const PIN_Config BoardGpioInitTable[] = {
    Board_3V3_EN     | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW    | PIN_PUSHPULL,                     /* 3V3 domain off initially      */
    Board_UART_RX    | PIN_INPUT_EN | PIN_PULLDOWN,                                                /* DevPack */
    Board_UART_TX    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH   | PIN_PUSHPULL,                     /* UART TX pin at inactive level */
    PIN_TERMINATE                                                                               /* Terminate list                */
};

const PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {
    .intPriority = ~0,
    .swiPriority = 0
};
/*============================================================================*/

/*
 *  ============================= Power begin ===================================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(PowerCC26XX_config, ".const:PowerCC26XX_config")
#endif

const PowerCC26XX_Config PowerCC26XX_config = {
    .policyInitFxn      = NULL,
    .policyFxn          = &PowerCC26XX_standbyPolicy,
    .calibrateFxn       = &PowerCC26XX_calibrate,
    .enablePolicy       = TRUE,
    .calibrateRCOSC_LF  = TRUE,
    .calibrateRCOSC_HF  = TRUE,
};
/*
 *  ============================= Power end ===================================
 */

/*
 *  ============================= UART begin ===================================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(UART_config, ".const:UART_config")
#pragma DATA_SECTION(uartCC26XXHWAttrs, ".const:uartCC26XXHWAttrs")
#endif

/* Include drivers */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

/* UART objects */
UARTCC26XX_Object uartCC26XXObjects[CC2650DK_5XD_UARTCOUNT];
unsigned char uartCC26XXRingBuffer[CC2650DK_5XD_UARTCOUNT][32];

/* UART hardware parameter structure, also used to assign UART pins */
const UARTCC26XX_HWAttrsV2 uartCC26XXHWAttrs[CC2650DK_5XD_UARTCOUNT] = {
    {
        .baseAddr       = UART0_BASE,
        .powerMngrId    = PowerCC26XX_PERIPH_UART0,
        .intNum         = INT_UART0_COMB,
        .intPriority    = ~0,
        .swiPriority    = 0,
        .txPin          = Board_UART_TX,
        .rxPin          = Board_UART_RX,
        .ctsPin         = PIN_UNASSIGNED,
        .rtsPin         = PIN_UNASSIGNED,
        .ringBufPtr     = uartCC26XXRingBuffer[0],
        .ringBufSize    = sizeof(uartCC26XXRingBuffer[0])
    }
};

/* UART configuration structure */
const UART_Config UART_config[] = {
    {
        .fxnTablePtr = &UARTCC26XX_fxnTable,
        .object      = &uartCC26XXObjects[0],
        .hwAttrs     = &uartCC26XXHWAttrs[0]
    },
    {NULL, NULL, NULL}
};
/*
 *  ============================= UART end =====================================
 */

/*
 *  ============================= UDMA begin ===================================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(UDMACC26XX_config, ".const:UDMACC26XX_config")
#pragma DATA_SECTION(udmaHWAttrs, ".const:udmaHWAttrs")
#endif

/* Include drivers */
#include <ti/drivers/dma/UDMACC26XX.h>

/* UDMA objects */
UDMACC26XX_Object udmaObjects[CC2650DK_5XD_UDMACOUNT];

/* UDMA configuration structure */
const UDMACC26XX_HWAttrs udmaHWAttrs[CC2650DK_5XD_UDMACOUNT] = {
    {
        .baseAddr    = UDMA0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_UDMA,
        .intNum      = INT_DMA_ERR,
        .intPriority = ~0
    }
};

/* UDMA configuration structure */
const UDMACC26XX_Config UDMACC26XX_config[] = {
    {
         .object  = &udmaObjects[0],
         .hwAttrs = &udmaHWAttrs[0]
    },
    {NULL, NULL}
};
/*
 *  ============================= UDMA end =====================================
 */


/*
 *  ========================== Crypto begin ====================================
 *  NOTE: The Crypto implementation should be considered experimental
 *        and not validated!
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(CryptoCC26XX_config, ".const:CryptoCC26XX_config")
#pragma DATA_SECTION(cryptoCC26XXHWAttrs, ".const:cryptoCC26XXHWAttrs")
#endif

/* Include drivers */
#include <ti/drivers/crypto/CryptoCC26XX.h>

/* Crypto objects */
CryptoCC26XX_Object cryptoCC26XXObjects[CC2650DK_5XD_CRYPTOCOUNT];

/* Crypto configuration structure, describing which pins are to be used */
const CryptoCC26XX_HWAttrs cryptoCC26XXHWAttrs[CC2650DK_5XD_CRYPTOCOUNT] = {
    {
        .baseAddr    = CRYPTO_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_CRYPTO,
        .intNum      = INT_CRYPTO_RESULT_AVAIL_IRQ,
        .intPriority = ~0,
    }
};

/* Crypto configuration structure */
const CryptoCC26XX_Config CryptoCC26XX_config[] = {
    {
        .object  = &cryptoCC26XXObjects[0],
        .hwAttrs = &cryptoCC26XXHWAttrs[0]
    },
    { NULL, NULL }
};
/*
 *  ========================== Crypto end =========================================
 */

/*
 *  ============================= I2C Begin=====================================
*/
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(I2C_config, ".const:I2C_config")
#pragma DATA_SECTION(i2cCC26xxHWAttrs, ".const:i2cCC26xxHWAttrs")
#endif

/* Include drivers */
#include <ti/drivers/i2c/I2CCC26XX.h>

/* I2C objects */
I2CCC26XX_Object i2cCC26xxObjects[CC2650DK_5XD_I2CCOUNT];

/* I2C configuration structure, describing which pins are to be used */
const I2CCC26XX_HWAttrsV1 i2cCC26xxHWAttrs[CC2650DK_5XD_I2CCOUNT] = {
    {
        .baseAddr = I2C0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_I2C0,
        .intNum = INT_I2C_IRQ,
        .intPriority = ~0,
        .swiPriority = 0,
        .sdaPin = Board_I2C0_SDA0,
        .sclPin = Board_I2C0_SCL0,
    }
};

const I2C_Config I2C_config[] = {
    {
        .fxnTablePtr = &I2CCC26XX_fxnTable,
        .object = &i2cCC26xxObjects[0],
        .hwAttrs = &i2cCC26xxHWAttrs[0]
    },
    {NULL, NULL, NULL}
};
/*
 *  ========================== I2C end =========================================
 */


/*
 *  ========================= RF driver begin ==============================================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(RFCC26XX_hwAttrs, ".const:RFCC26XX_hwAttrs")
#endif

/* Include drivers */
#include <ti/drivers/rf/RF.h>

/* RF hwi and swi priority */
const RFCC26XX_HWAttrs RFCC26XX_hwAttrs = {
    .hwiCpe0Priority = ~0,
    .hwiHwPriority   = ~0,
    .swiCpe0Priority =  0,
    .swiHwPriority   =  0,
};

/*
 *  ========================== RF driver end =========================================
 */

/*
 *  ========================= Display begin ====================================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(Display_config, ".const:Display_config")
#pragma DATA_SECTION(displayUartHWAttrs, ".const:displayUartHWAttrs")
#endif

#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayUart.h>

/* Structures for UartPlain Blocking */
DisplayUart_Object        displayUartObject;

#ifndef BOARD_DISPLAY_UART_STRBUF_SIZE
#define BOARD_DISPLAY_UART_STRBUF_SIZE    128
#endif
static char uartStringBuf[BOARD_DISPLAY_UART_STRBUF_SIZE];

const DisplayUart_HWAttrs displayUartHWAttrs = {
    .uartIdx      = Board_UART,
    .baudRate     =     115200,
    .mutexTimeout = BIOS_WAIT_FOREVER,
    .strBuf = uartStringBuf,
    .strBufLen = BOARD_DISPLAY_UART_STRBUF_SIZE,
};

/* Array of displays */
const Display_Config Display_config[] = {
#if !defined(BOARD_DISPLAY_EXCLUDE_UART)
    {
        .fxnTablePtr = &DisplayUart_fxnTable,
        .object      = &displayUartObject,
        .hwAttrs     = &displayUartHWAttrs,
    },
#endif
    { NULL, NULL, NULL } // Terminator
};

/*
 *  ========================= Display end ======================================
 */

/*
 *  ============================ GPTimer begin =================================
 *  Remove unused entries to reduce flash usage both in Board.c and Board.h
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(GPTimerCC26XX_config, ".const:GPTimerCC26XX_config")
#pragma DATA_SECTION(gptimerCC26xxHWAttrs, ".const:gptimerCC26xxHWAttrs")
#endif

/* GPTimer hardware attributes, one per timer part (Timer 0A, 0B, 1A, 1B..) */
const GPTimerCC26XX_HWAttrs gptimerCC26xxHWAttrs[CC2650DK_5XD_GPTIMERPARTSCOUNT] = {
    { .baseAddr = GPT0_BASE, .intNum = INT_GPT0A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0A, },
    { .baseAddr = GPT0_BASE, .intNum = INT_GPT0B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0B, },
    { .baseAddr = GPT1_BASE, .intNum = INT_GPT1A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT1, .pinMux = GPT_PIN_1A, },
    { .baseAddr = GPT1_BASE, .intNum = INT_GPT1B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT1, .pinMux = GPT_PIN_1B, },
    { .baseAddr = GPT2_BASE, .intNum = INT_GPT2A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT2, .pinMux = GPT_PIN_2A, },
    { .baseAddr = GPT2_BASE, .intNum = INT_GPT2B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT2, .pinMux = GPT_PIN_2B, },
    { .baseAddr = GPT3_BASE, .intNum = INT_GPT3A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT3, .pinMux = GPT_PIN_3A, },
    { .baseAddr = GPT3_BASE, .intNum = INT_GPT3B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT3, .pinMux = GPT_PIN_3B, },
};

/*  GPTimer objects, one per full-width timer (A+B) (Timer 0, Timer 1..) */
GPTimerCC26XX_Object gptimerCC26XXObjects[CC2650DK_5XD_GPTIMERCOUNT];

/* GPTimer configuration (used as GPTimer_Handle by driver and application) */
const GPTimerCC26XX_Config GPTimerCC26XX_config[CC2650DK_5XD_GPTIMERPARTSCOUNT] = {
    { &gptimerCC26XXObjects[0], &gptimerCC26xxHWAttrs[0], GPT_A },
    { &gptimerCC26XXObjects[0], &gptimerCC26xxHWAttrs[1], GPT_B },
    { &gptimerCC26XXObjects[1], &gptimerCC26xxHWAttrs[2], GPT_A },
    { &gptimerCC26XXObjects[1], &gptimerCC26xxHWAttrs[3], GPT_B },
    { &gptimerCC26XXObjects[2], &gptimerCC26xxHWAttrs[4], GPT_A },
    { &gptimerCC26XXObjects[2], &gptimerCC26xxHWAttrs[5], GPT_B },
    { &gptimerCC26XXObjects[3], &gptimerCC26xxHWAttrs[6], GPT_A },
    { &gptimerCC26XXObjects[3], &gptimerCC26xxHWAttrs[7], GPT_B },
};

/*
 *  ============================ GPTimer end ===================================
 */

/*
 *  =============================== Watchdog ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(Watchdog_config, ".const:Watchdog_config")
#pragma DATA_SECTION(watchdogCC26XXHWAttrs, ".const:watchdogCC26XXHWAttrs")
#endif

#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogCC26XX.h>

WatchdogCC26XX_Object watchdogCC26XXObjects[CC2650DK_5XD_WATCHDOGCOUNT];

const WatchdogCC26XX_HWAttrs watchdogCC26XXHWAttrs[CC2650DK_5XD_WATCHDOGCOUNT] = {
    {
        .baseAddr = WDT_BASE,
        .intNum = INT_WDT_IRQ,
        .reloadValue = 1000 /* Reload value in milliseconds */
    },
};

const Watchdog_Config Watchdog_config[] = {
    {
        .fxnTablePtr = &WatchdogCC26XX_fxnTable,
        .object = &watchdogCC26XXObjects[0],
        .hwAttrs = &watchdogCC26XXHWAttrs[0]
    },
    {NULL, NULL, NULL},
};

/*
 *  ======== CC26XX_LAUNCHXL_initWatchdog ========
 */
void CC26XX_LAUNCHXL_initWatchdog(void)
{
    Watchdog_init();
}
