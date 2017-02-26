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
        uint8_t begin();
        uint8_t getVoltage(float &voltage);
        uint8_t engineLoad(uint8_t &load);

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
        uint8_t vehicleSpeed(uint8_t &speed);

        /**
         * Gets the current throttle position. Reads PID 11 from the OBD interface
         * and sets position to the value returned after conversion.
         * @param[out] position uint8_t containing throttle position.
         * - Minimum Value: 0
         * - Maximum Value: 100
         * - %
         */
        uint8_t throttlePosition(uint8_t &position);
        uint8_t auxiliaryInputStatus(bool &auxStatus);
        uint8_t engineRunTime(unsigned int &runTime);
        uint8_t distanceMIL(unsigned int &distance);
        uint8_t commandedEGR(uint8_t &egr);
        uint8_t EGRError(int &error);
        uint8_t fuelLevel(uint8_t &level);
        uint8_t warmUpsSinceLastCleared(uint8_t &warmUps);
        uint8_t distanceSinceLastCleared(unsigned int &distance);
        uint8_t evapPressure(int &pressure);
        uint8_t controlModuleVoltage(unsigned int &voltage);
        uint8_t absoluteLoadValue(unsigned int &load);
        uint8_t commandEquivalenceRatio(float &ratio);
        uint8_t relativeThrottlePosition(uint8_t &position);
        uint8_t absoluteThrottlePositionB(uint8_t &position);
        uint8_t absoluteThrottlePositionC(uint8_t &position);
        uint8_t acceleratorPedalPositionD(uint8_t &position);
        uint8_t acceleratorPedalPositionE(uint8_t &position);
        uint8_t acceleratorPedalPositionF(uint8_t &position);
        uint8_t commandedThrottleActuator(uint8_t &position);
    private:
        uint8_t o2WRVoltage(const char *sensor, unsigned int &equivRatio, unsigned int &voltage);
        uint8_t o2WRCurrent(const char *sensor, unsigned int &equivRatio, int &current);
        uint8_t catTemperature(const char *sensor, int &temperature);
        uint8_t getuint8_ts( const char *mode, const char *chkMode, const char *pid, uint8_t *values, unsigned int numValues);
        uint8_t runCommand(const char *cmd, char *data, unsigned int dataLength);
        bool getBit(uint8_t b, uint8_t p);
        uint8_t getFuelTrim(const char *pid, int &percent);
        void flush();
};

