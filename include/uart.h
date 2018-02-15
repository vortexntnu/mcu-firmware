#ifndef _INCL_UART
#define _INCL_UART
#endif

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "efm32gg990f1024.h"

#define COM_PORT gpioPortD
#define UART USART1
#define UART_TX_pin 0
#define UART_RX_pin 1

void initUart(void);
