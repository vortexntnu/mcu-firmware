#include "rov_utilities.h"

struct vortex_msg
{
	uint8_t 	magic_start;				// start transmission byte
	uint8_t 	type;						// message type
	uint8_t 	payload[MAX_PAYLOAD_SIZE];	// payload
	uint8_t 	crc_byte1;         			// crc byte
	uint8_t 	crc_byte2;					// crc byte
	uint8_t		magic_stop;					// stop transmission byte
} vortex_message = { MAGIC_START_BYTE, 0, {0}, 0, 0, MAGIC_STOP_BYTE };

bool start_sequence_finished = false;
uint16_t start_sequence_passed_ms = 0;

bool arm_sequence_finished = false;
uint16_t arm_sequence_passed_ms = 0;

bool disarm_sequence_finished = false;
uint16_t disarm_sequence_passed_ms = 0;


uint32_t us_to_comparevalue(uint32_t us)
{
	uint32_t hz_to_us = 1000000 / THRUSTER_PWM_FREQ;

	if((us < THRUSTER_MIN_PULSE_WIDTH_US) || (us > THRUSTER_MAX_PULSE_WIDTH_US))
	{
		return THRUSTER_START_PULSE_WIDTH_US;
		//return ((CMU_ClockFreqGet(cmuClock_HFPER) / THRUSTER_PWM_FREQ) * THRUSTER_START_PULSE_WIDTH_US) / hz_to_us;
	}

	return ((CMU_ClockFreqGet(cmuClock_HFPER) / THRUSTER_PWM_FREQ) * us) / hz_to_us;
}


uint8_t update_thruster_pwm(uint8_t *pwm_data_ptr)
{
	int i;
	uint16_t pwm_data[NUM_THRUSTERS];

	// convert 16 x two bytes to 8 x uint16_t
	for (i = 0; i < NUM_THRUSTERS; i++)
	{
		pwm_data[i] = (uint16_t)((*pwm_data_ptr << 8) & 0xFF00);	// msb
		pwm_data_ptr++;
		pwm_data[i] |= (uint16_t)((*pwm_data_ptr) & 0x00FF);		// lsb
		pwm_data_ptr++;
	}

	int ch;

	for (ch = 0; ch < 3; ch++)
	{
		TIMER_CompareBufSet(TIMER0, ch, us_to_comparevalue(pwm_data[ch]));
		TIMER_CompareBufSet(TIMER1, ch, us_to_comparevalue(pwm_data[ch + 3]));
		TIMER_CompareBufSet(TIMER2, ch, us_to_comparevalue(pwm_data[ch + 5]));
	}

	return PWM_UPDATE_OK;
}


uint8_t update_led_pwm(uint8_t *pwm_data_ptr)
{
	return PWM_UPDATE_OK;
}


void start_sequence(void)
{
	NVIC_EnableIRQ(LETIMER0_IRQn);
	NVIC_DisableIRQ(USART1_RX_IRQn);

	uint8_t pwm_signals[NUM_THRUSTERS * 2] = {0};

	char uart_msg[50] = {0};
	char* uart_msg_ptr = &uart_msg[0];

	start_sequence_finished = false;
	disarm_sequence_finished = false;
	arm_sequence_finished = false;

	while (start_sequence_finished == false)
	{
		update_thruster_pwm(&pwm_signals[0]);
		WDOGn_Feed(WDOG);
	}

	GPIO_PinOutSet(LED1_PORT, LED1_PIN);
	GPIO_PinOutSet(LED2_PORT, LED2_PIN);

	strcpy(uart_msg_ptr, "MCU initialization finished...\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	NVIC_EnableIRQ(USART1_RX_IRQn);

}


void disarm_sequence(void)
{

	NVIC_DisableIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);

	char uart_msg[50] = {0};
	char* uart_msg_ptr = &uart_msg[0];

	uint8_t pwm_signals[NUM_THRUSTERS * 2] = {0};

	strcpy(uart_msg_ptr, "DISARMING start\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	while (disarm_sequence_finished == false)
	{
		update_thruster_pwm(&pwm_signals[0]);
		WDOGn_Feed(WDOG);
	}

	strcpy(uart_msg_ptr, "DISARMING finished\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	arm_sequence_finished = false;

	NVIC_EnableIRQ(USART1_RX_IRQn);

}


void arm_sequence(void)
{
	NVIC_DisableIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);

	char uart_msg[50] = {0};
	char* uart_msg_ptr = &uart_msg[0];

	strcpy(uart_msg_ptr, "ARMING start\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	uint8_t pwm_signals[NUM_THRUSTERS * 2];

	// convert 0b10111011100 (1500) to two bytes
	for (int i = 0; i < NUM_THRUSTERS; i++)
	{
		pwm_signals[i*2] = 0b00000101;		// high bits
		pwm_signals[i*2 + 1]  = 0b11011100; // low bits
	}

	while (arm_sequence_finished == false)
	{
		update_thruster_pwm(&pwm_signals[0]);
		WDOGn_Feed(WDOG);
	}

	strcpy(uart_msg_ptr, "ARMING finished\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	disarm_sequence_finished = false;

	NVIC_EnableIRQ(USART1_RX_IRQn);

}


void LETIMER0_IRQHandler(void)
{
	if (start_sequence_finished == false)
	{
		start_sequence_passed_ms += LETIMER_MS;
	}

	if (start_sequence_passed_ms >= START_SEQUENCE_DURATION_MS)
	{
			start_sequence_finished = true;
			NVIC_DisableIRQ(LETIMER0_IRQn);
	}

	if (arm_sequence_finished == false)
	{
		arm_sequence_passed_ms += LETIMER_MS;
	}

	if (arm_sequence_passed_ms >= ARM_SEQUENCE_DURATION_MS)
	{
		arm_sequence_finished = true;
		NVIC_DisableIRQ(LETIMER0_IRQn);

	}

	if (disarm_sequence_finished == false)
	{
		disarm_sequence_passed_ms += LETIMER_MS;
	}

	if (disarm_sequence_passed_ms >= DISARM_SEQUENCE_DURATION_MS)
	{
		disarm_sequence_finished = true;
		NVIC_DisableIRQ(LETIMER0_IRQn);

	}

	LETIMER_IntClear(LETIMER0, LETIMER_IF_UF);
	LETIMER0->CNT = LETIMER_MS;

	GPIO_PinOutToggle(LED1_PORT, LED1_PIN);
	GPIO_PinOutToggle(LED2_PORT, LED2_PIN);
}

bool crc_passed(uint8_t * receive_data)
{
	uint16_t crc_checksum_calc = 0;
	uint16_t crc_checksum_received = 1;

	crc_checksum_calc = crc_16(&receive_data[VORTEX_MSG_START_DATA_INDEX], MAX_PAYLOAD_SIZE);
	crc_checksum_received = (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX] << 8) & 0xFF00)
							| (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX+1]) & 0x00FF);

	if (crc_checksum_calc == crc_checksum_received)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void send_vortex_msg(msg_type type)
{
	switch(type)
	{
		case MSG_TYPE_NOACK:
			vortex_message.type = MSG_TYPE_NOACK;
			strcpy((char*)&vortex_message.payload[0], "NO ACK");
			break;
		case MSG_TYPE_ACK:
			vortex_message.type = MSG_TYPE_ACK;
			strcpy((char*)&vortex_message.payload, "ACK!");
			break;
		default:
			vortex_message.type = MSG_TYPE_NOTYPE;
			break;
	}

	USART_Tx(UART, vortex_message.magic_start);
	USART_Tx(UART, vortex_message.type);
	USART_PutData((uint8_t*) &vortex_message.payload, (uint8_t)strlen((char*)vortex_message.payload));
	USART_Tx(UART, vortex_message.magic_stop);
	USART_PutData((uint8_t*)"\n\r", 2);

}


void initLeTimer(void)
{
	const LETIMER_Init_TypeDef letimerInit =
	{
	  .enable         = false,            	// Start counting when init completed
	  .debugRun       = true,             	// Counter shall not keep running during debug halt
	  .comp0Top       = true,             	// Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP
	  .bufTop         = false,            	// Don't load COMP1 into COMP0 when REP0 reaches 0
	  .out0Pol        = 0,                	// Idle value for output 0
	  .out1Pol        = 0,                	// Idle value for output 1
	  .ufoa0          = letimerUFOAToggle,	// Toggle output on output 0
	  .ufoa1          = letimerUFOAToggle,	// Toggle output on output 1
	  .repMode        = letimerRepeatFree 	// Count until stopped
	};

	CMU_ClockEnable(cmuClock_HFLE, true);

	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);

	// The clock source of LETIMER0
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

	CMU_ClockEnable(cmuClock_LETIMER0, true);

	// Time out value configuration
	LETIMER_CompareSet(LETIMER0, 0, LETIMER_MS);

	// Initializing LETIMER0
	LETIMER_Init(LETIMER0, &letimerInit);
	LETIMER_Enable(LETIMER0, true);

	LETIMER_IntClear(LETIMER0, _LETIMER_IF_MASK);
	LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_DisableIRQ(LETIMER0_IRQn);
}
