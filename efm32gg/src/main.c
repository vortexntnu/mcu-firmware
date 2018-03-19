#include <string.h>

#include "uart.h"
#include "pwm.h"
#include "vortex_msg.h"
#include "efm32gg990f1024.h"

int main() {

	CHIP_Init();

	initUart();
	initPwm();

	uint32_t i;

	uint8_t receive_data[VORTEX_MSG_MAX_SIZE] = {0};
	uint8_t *receive_data_ptr = &receive_data[0];

	uint8_t msg_type = MSG_TYPE_NOTYPE;

	char hello_world[] = "\n\rHello World!\n\r";

	for (i = 0; i < strlen(hello_world); i++)
	{
		USART_Tx(UART, hello_world[i]);
	}

	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortE, 3);

	while (1)
	{
		switch (receive_vortex_msg(receive_data_ptr))
		{
			case MSG_STATE_MAGIC_BYTES_NOT_RECEIVED:
				msg_type = MSG_TYPE_NOTYPE;
				break;

			case MSG_STATE_RECEIVE_OK:
				msg_type = receive_data[VORTEX_MSG_TYPE_INDEX];
				send_vortex_msg(MSG_TYPE_ACK);
				break;

			case MSG_STATE_RECEIVE_FAIL:
				msg_type = MSG_TYPE_NOTYPE;
				send_vortex_msg(MSG_TYPE_NOACK);
				break;

			default:
				msg_type = MSG_TYPE_NOTYPE;
				send_vortex_msg(MSG_TYPE_NOACK);
				break;
		}

		switch(msg_type)
		{
			case MSG_TYPE_NOTYPE:
				break;

			case MSG_TYPE_THRUSTER:
				update_thruster_pwm(&receive_data[VORTEX_MSG_START_DATA_INDEX]);
				break;

			case MSG_TYPE_LED:
				break;

			default:
				break;
		}

		for (i = 1; i <= receive_data[0]; i++)
		{
			USART_Tx(UART, receive_data[i]);
		}

		for (i = 0; i < VORTEX_MSG_MAX_SIZE; i++)
		{
			receive_data[i] = 0;
		}
	}
}
