#include <string.h>

#include "uart.h"
#include "pwm.h"
#include "efm32gg990f1024.h"
int main() {

	CHIP_Init();

	initUart();
	//init_Pwm();

	uint32_t i,c, d;
	char hello_world[] = "\n\rHello World!\n\r";

	uint16_t receive_data;

	for(i=0; i<strlen(hello_world); i++) USART_Tx(UART, hello_world[i]);

	while(1){

		uartGetData(&receive_data);

		for(c = 0; c <=3500; c++)
			for(d = 0; d <=350; d++);

		//generate_PWM();
	}

}
