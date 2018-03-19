#include <string.h>

#include "uart.h"
#include "pwm.h"
#include "efm32gg990f1024.h"

int main() {

	CHIP_Init();

	initUart();
	initPwm();

	uint32_t i;

	char hello_world[] = "\n\rHello World!\n\r";

	for (i = 0; i < strlen(hello_world); i++)
	{
		USART_Tx(UART, hello_world[i]);
	}

	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortE, 3);

	while (1)
	{
		switch (receive_vortex_msg())
		{
			case MAGIC_BYTES_NOT_RECEIVED:
				break;
			case RECEIVE_OK:
				send_vortex_msg(ACK_MSG);
				break;
			case RECEIVE_FAIL:
				send_vortex_msg(NOACK_MSG);
				break;
			default:
				break;
		}

	}

}
