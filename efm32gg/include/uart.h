#ifndef UART_H_
#define UART_H_


#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "efm32gg990f1024.h"

#define BUFFERSIZE 20

extern volatile struct circularBuffer receiveBuff, transmitBuff;

#define UART USART1
#define UART_PORT gpioPortD
#define UART_LOCATION UART_ROUTE_LOCATION_LOC2
#define UART_TX_PIN 7
#define UART_RX_PIN 6

void initUart(void);
void setupUart(void);
void uartGetData(uint8_t* dataPtr);
void uartPutData(uint8_t* dataPtr);

#endif //UART_H_
