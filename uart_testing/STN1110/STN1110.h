#define ELM_SUCCESS 0

#define ELM_NO_RESPONSE 1

#define ELM_BUFFER_OVERFLOW 2

#define ELM_GARBAGE 3

#define ELM_UNABLE_TO_CONNECT 4

#define ELM_NO_DATA 5

#include <stdint.h>
#include <ti/drivers/UART.h>
class STN1110
{
    public:
        uint8_t firstTime;
        UART_Handle uart;
        UART_Params uartParams;
        uint8_t elmCheck();
        uint8_t begin();
        uint8_t engineLoad(uint8_t &load);
        uint8_t random;

        /**
         * Gets the current engine RPM.  Reads PID 0C from the OBD Interface and
         * sets rpm to the value returned after conversion.
         * @param[out]  rpm Unsigned integer is set to the current engine rpm.
         * - Minimum Value: 0
         * - Maximum Value: 16,383
         * - Units: RPM
         */
        uint8_t engineRPM(int &rpm);
        /**
         * Gets the current speed of the vehicle in km/h.  Reads PID 0D form the OBD
         * interface and sets speed to the value returned after conversion.
         * @param[out]  speed   uint8_t is set to the current speed of the vehicle.
         * - Minimum Value: 0
         * - Maximum Value: 255
         * - Units: km/h
         */
        uint8_t getSpeed();
        /**
         * Gets the current throttle position. Reads PID 11 from the OBD interface
         * and sets position to the value returned after conversion.
         * @param[out] position uint8_t containing throttle position.
         * - Minimum Value: 0
         * - Maximum Value: 100
         * - %
         */
        uint8_t getThrottlePosition();
};

