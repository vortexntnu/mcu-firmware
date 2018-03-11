#ifndef UART_H_
#define UART_H_


#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "efm32gg990f1024.h"

#define BUFFERSIZE 10

extern volatile struct circularBuffer receiveBuff, transmitBuff;

#define COM_PORT gpioPortD
#define USART_LOCATION 1
#define USART_TX_pin 0
#define USART_RX_pin 1

void initUart(void);
void uartGetData(uint8_t* dataPtr);
void uartPutData(uint8_t* dataPtr);

#endif //UART_H_
