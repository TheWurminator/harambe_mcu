
#include <stdint.h>
#include <ti/drivers/UART.h>

#define ELM_SUCCESS 0

#define ELM_NO_RESPONSE 1

#define ELM_BUFFER_OVERFLOW 2

#define ELM_GARBAGE 3

#define ELM_UNABLE_TO_CONNECT 4

#define ELM_NO_DATA 5

typedef struct{
    uint8_t firstTime;
    UART_Handle uart;
    UART_Params uartParams;
    uint8_t random;
}STN1110;

uint8_t elmCheck(STN1110 *);
uint8_t begin(STN1110 *);
uint8_t init(STN1110 *);
uint8_t getSpeed(STN1110*);
uint8_t transform(char x);
