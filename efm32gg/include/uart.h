#ifndef UART_H_
#define UART_H_

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "efm32gg990f1024.h"

enum msg_state
{
	MAGIC_BYTES_NOT_RECEIVED,
	MAGIC_BYTES_RECEIVED,
	SEND_OK,
	SEND_FAIL,
	RECEIVE_OK,
	RECEIVE_FAIL,
};

enum msg_type
{
	ACK_MSG,
	NOACK_MSG,
};

#define BUFFERSIZE 64
#define MAX_PAYLOAD_SIZE 16
#define VORTEX_MSG_MAX_SIZE 21

extern struct circularBuffer receiveBuff;
extern struct vortex_msg message;

#define MAGIC_START_BYTE 	0x69
#define MAGIC_STOP_BYTE 	0x21
#define MSG_TYPE_THRUSTER 	0x70

#define UART USART1
#define UART_PORT gpioPortD
#define UART_LOCATION UART_ROUTE_LOCATION_LOC2
#define UART_TX_PIN 7
#define UART_RX_PIN 6

void initUart(void);
void setupUart(void);
uint8_t magic_bytes_received(void);
uint8_t receive_vortex_msg(void);
uint8_t send_vortex_msg(enum msg_type type);

#endif //UART_H_
