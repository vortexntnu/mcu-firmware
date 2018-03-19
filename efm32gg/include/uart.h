#ifndef UART_H_
#define UART_H_

#include "vortex_msg.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "efm32gg990f1024.h"

#define BUFFERSIZE 64

extern struct circularBuffer receiveBuff;

#define UART USART1
#define UART_PORT gpioPortD
#define UART_LOCATION UART_ROUTE_LOCATION_LOC2
#define UART_TX_PIN 7
#define UART_RX_PIN 6

void initUart(void);
void setupUart(void);
uint8_t magic_bytes_received(void);
uint8_t receive_vortex_msg(uint8_t *receive_data_ptr);
uint8_t send_vortex_msg(msg_flag type);

#endif //UART_H_
