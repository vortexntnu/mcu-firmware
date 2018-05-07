#include "rov_utilities.h"

struct thruster_config
{
	TIMER_TypeDef * timer[NUM_THRUSTERS];		// Timers the thruster pins are connected to
	uint8_t			cc[NUM_THRUSTERS];			// Compare channel thrusters
	uint32_t 		location[NUM_THRUSTERS];	// Pinout location
	uint8_t			mapping[NUM_THRUSTERS];		// Mapping of the thrusters
} thruster = {
	{THR0_TIM, 	THR1_TIM, 	THR2_TIM,	THR3_TIM,	THR4_TIM,	THR5_TIM,	THR6_TIM, 	THR7_TIM},
	{THR0_CC, 	THR1_CC,	THR2_CC, 	THR3_CC, 	THR4_CC, 	THR5_CC, 	THR6_CC, 	THR7_CC	},
	{THR0_LOC, 	THR1_LOC,	THR2_LOC, 	THR3_LOC, 	THR4_LOC, 	THR5_LOC, 	THR6_LOC,	THR7_LOC},
	{THR0,		THR1, 		THR2, 		THR3, 		THR4, 		THR5, 		THR6,		THR7	}};

struct vortex_msg
{
	uint8_t 	magic_start;				// start transmission byte
	uint8_t 	type;					// message type
	uint8_t 	payload[MAX_PAYLOAD_SIZE];	// payload
	uint8_t 	crc_byte1;         		// crc byte
	uint8_t 	crc_byte2;				// crc byte
	uint8_t	magic_stop;				// stop transmission byte
} vortex_message = { MAGIC_START_BYTE, 0, {0}, 0, 0, MAGIC_STOP_BYTE };

bool sequence_finished = false;
bool rov_armed = false;
uint16_t sequence_passed_ms = 0;
uint8_t sequence_type = SEQUENCE_START;

uint32_t us_to_comparevalue(uint32_t us, TIMER_TypeDef *timer)
{
	return (TIMER_TopGet(timer) / THRUSTER_PWM_PERIOD) * us;
}


uint8_t update_thruster_pwm(uint8_t *pwm_data_ptr)
{
	if (rov_armed == true)
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

			if((pwm_data[i] < THRUSTER_MIN_PULSE_WIDTH_US) || (pwm_data[i] > THRUSTER_MAX_PULSE_WIDTH_US))
			{
				pwm_data[i] = THRUSTER_START_PULSE_WIDTH_US;
			}

		}

		int thr = 0;

		for (thr = 0; thr < NUM_THRUSTERS; thr++)
		{
			TIMER_CompareBufSet(thruster.timer[thr],
								thruster.cc[thr],
								us_to_comparevalue(pwm_data[thruster.mapping[thr]], thruster.timer[thr]));
		}

		return PWM_UPDATE_OK;
	}
	else
	{

		int thr = 0;

		for (thr = 0; thr < NUM_THRUSTERS; thr++)
		{
			TIMER_CompareBufSet(thruster.timer[thruster.mapping[thr]],
								thruster.cc[thruster.mapping[thr]],
								us_to_comparevalue(THRUSTER_START_PULSE_WIDTH_US, thruster.timer[thruster.mapping[thr]]));
		}

		return PWM_UPDATE_FAIL;
	}
}


uint8_t update_light_pwm(uint8_t *pwm_data_ptr)
{
	uint16_t pwm_data;

	// convert 2 x two bytes to 1 x uint16_t
	pwm_data = (uint16_t)((*pwm_data_ptr << 8) & 0xFF00);	// msb
	pwm_data_ptr++;
	pwm_data |= (uint16_t)((*pwm_data_ptr) & 0x00FF);		// lsb
	pwm_data_ptr++;

	TIMER_CompareBufSet(LIGHT_TIM, LIGHT_CC, us_to_comparevalue(pwm_data, LIGHT_TIM));

	return PWM_UPDATE_OK;
}


void start_sequence(void)
{
	NVIC_EnableIRQ(LETIMER0_IRQn);
	NVIC_DisableIRQ(USART1_RX_IRQn);

	uint8_t pwm_signals[NUM_THRUSTERS * 2] = {0};

	char uart_msg[50] = {0};
	char* uart_msg_ptr = &uart_msg[0];

	sequence_type = SEQUENCE_START;
	sequence_passed_ms = 0;

	rov_armed = true;

	while (sequence_finished == false)
	{
		update_thruster_pwm(&pwm_signals[0]);
		update_light_pwm(&pwm_signals[0]);
		WDOGn_Feed(WDOG);
	}

	GPIO_PinOutSet(LED1_PORT, LED1_PIN);
	GPIO_PinOutClear(LED2_PORT, LED2_PIN);

	sequence_finished = false;
	rov_armed = false;

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

	sequence_type = SEQUENCE_DISARM;
	sequence_passed_ms = 0;

	strcpy(uart_msg_ptr, "DISARMING start\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	while (sequence_finished == false)
	{
		update_thruster_pwm(&pwm_signals[0]);
		update_light_pwm(&pwm_signals[0]);
		WDOGn_Feed(WDOG);
	}

	sequence_finished = false;
	rov_armed = false;

	strcpy(uart_msg_ptr, "DISARMING finished\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

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

	uint8_t pwm_signals[NUM_THRUSTERS * 2] = {0};
	sequence_type = SEQUENCE_ARM;
	sequence_passed_ms = 0;

	// convert 0b10111011100 (1500) to two bytes
	for (int i = 0; i < NUM_THRUSTERS; i++)
	{
		pwm_signals[i*2] = 0b00000101;		// high bits
		pwm_signals[i*2 + 1]  = 0b11011100; // low bits
	}

	rov_armed = true;

	while (sequence_finished == false)
	{
		update_thruster_pwm(&pwm_signals[0]);
		update_light_pwm(&pwm_signals[0]);
		WDOGn_Feed(WDOG);
	}

	sequence_finished = false;


	strcpy(uart_msg_ptr, "ARMING finished\n\r");
	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	NVIC_EnableIRQ(USART1_RX_IRQn);
}


void LETIMER0_IRQHandler(void)
{
	sequence_passed_ms += LETIMER_MS;
	switch(sequence_type)
	{
		case SEQUENCE_START:
			if(sequence_passed_ms >= START_SEQUENCE_DURATION_MS)
			{
				sequence_finished = true;
				sequence_passed_ms = 0;
				sequence_type = NO_SEQUENCE;
				NVIC_DisableIRQ(LETIMER0_IRQn);
			}
			break;

		case SEQUENCE_ARM:
			if(sequence_passed_ms >= ARM_SEQUENCE_DURATION_MS)
			{
				sequence_finished = true;
				sequence_passed_ms = 0;
				sequence_type = NO_SEQUENCE;
				NVIC_DisableIRQ(LETIMER0_IRQn);
			}
			break;

		case SEQUENCE_DISARM:
			if(sequence_passed_ms >= ARM_SEQUENCE_DURATION_MS)
			{
				sequence_finished = true;
				sequence_passed_ms = 0;
				sequence_type = NO_SEQUENCE;
				NVIC_DisableIRQ(LETIMER0_IRQn);
			}
			break;

		default:
			break;
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

	switch(receive_data[VORTEX_MSG_TYPE_INDEX])
	{
		case MSG_TYPE_THRUSTER:
			crc_checksum_calc = crc_16(&receive_data[VORTEX_MSG_START_DATA_INDEX], MAX_PAYLOAD_SIZE);
			crc_checksum_received = (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX] << 8) & 0xFF00)
										| (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX+1]) & 0x00FF);
			break;

		case MSG_TYPE_LIGHT:
			crc_checksum_calc = crc_16(&receive_data[VORTEX_MSG_START_DATA_INDEX], LIGHT_PAYLOAD_SIZE);
			crc_checksum_received = (uint16_t)((receive_data[LIGHT_MSG_SIZE-2] << 8) & 0xFF00)
										| (uint16_t)((receive_data[LIGHT_MSG_SIZE - 1]) & 0x00FF);
			break;

		case MSG_TYPE_ARM:
			return true;

		case MSG_TYPE_DISARM:
			return true;

		case MSG_TYPE_HEARTBEAT:
			return true;

		default:
			return false;
	}

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
