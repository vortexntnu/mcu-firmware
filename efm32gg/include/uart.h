#ifndef UART_H
#define UART_H

#include "rov_utilities.h"

#include <string.h>

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "efm32gg_uart.h"
#include "efm32gg942f1024.h"

#define BUFFERSIZE 128

extern struct circularBuffer receiveBuff;

#define UART USART1
#define UART_PORT gpioPortD
#define UART_LOCATION UART_ROUTE_LOCATION_LOC2
#define UART_TX_PIN 7
#define UART_RX_PIN 6

void initUart(void);
void USART_PutData(uint8_t * data_ptr, uint8_t size);
uint8_t magic_bytes_received(void);
uint8_t receive_vortex_msg(uint8_t *receive_data_ptr);

#endif //UART_H_
