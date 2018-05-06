#ifndef ROV_UTILITIES_H
#define ROV_UTILITIES_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "em_rmu.h"
#include "em_letimer.h"

#include "pwm.h"
#include "crc.h"
#include "uart.h"
#include "watchdog.h"

#define THR0_PORT 	gpioPortD
#define THR0_PIN	3
#define THR0_TIM	TIMER0
#define THR0_CC		2
#define THR0_LOC	TIMER_ROUTE_LOCATION_LOC3

#define THR1_PORT	gpioPortB
#define THR1_PIN	11
#define THR1_TIM	TIMER1
#define THR1_CC		2
#define THR1_LOC	TIMER_ROUTE_LOCATION_LOC3

#define THR2_PORT	gpioPortD
#define THR2_PIN	1
#define THR2_TIM	TIMER0
#define THR2_CC		0
#define THR2_LOC	TIMER_ROUTE_LOCATION_LOC3

#define THR3_PORT	gpioPortD
#define THR3_PIN	2
#define THR3_TIM	TIMER0
#define THR3_CC		1
#define THR3_LOC	TIMER_ROUTE_LOCATION_LOC3

#define THR4_PORT	gpioPortA
#define THR4_PIN	12
#define THR4_TIM	TIMER2
#define THR4_CC		0
#define THR4_LOC	TIMER_ROUTE_LOCATION_LOC1

#define THR5_PORT	gpioPortA
#define THR5_PIN	13
#define THR5_TIM	TIMER2
#define THR5_CC		1
#define THR5_LOC	TIMER_ROUTE_LOCATION_LOC1

#define THR6_PORT	gpioPortB
#define THR6_PIN	7
#define THR6_TIM	TIMER1
#define THR6_CC		0
#define THR6_LOC	TIMER_ROUTE_LOCATION_LOC3

#define THR7_PORT	gpioPortB
#define THR7_PIN	8
#define THR7_TIM	TIMER1
#define THR7_CC		1
#define THR7_LOC	TIMER_ROUTE_LOCATION_LOC3

#define LIGHT_PORT	gpioPortA
#define LIGHT_PIN	14
#define LIGHT_TIM	TIMER3
#define LIGHT_CC	0

#define EXTRA_PWM_PORT	gpioPortE
#define EXTRA_PWM_PIN	14


#define MAGIC_START_BYTE 				0x24
#define MAGIC_STOP_BYTE 				0x40

#define MAX_PAYLOAD_SIZE 				16
#define VORTEX_MSG_MAX_SIZE 			21
#define VORTEX_MSG_TYPE_INDEX 			2
#define VORTEX_MSG_START_DATA_INDEX 		3
#define VORTEX_MSG_CRC_BYTE_INDEX 		19

#define LIGHT_MSG_SIZE					7
#define LIGHT_PAYLOAD_SIZE				2

#define TYPE_ONLY_MSG_SIZE				3

#define LETIMER_MS 					100		// how often LETIMER0_IRQHandler triggers in milliseconds
#define ARM_SEQUENCE_DURATION_MS 		3000
#define DISARM_SEQUENCE_DURATION_MS		2000
#define START_SEQUENCE_DURATION_MS 		2000

#define LED1_PORT 						gpioPortE
#define LED1_PIN  						12
#define LED2_PORT 						gpioPortE
#define LED2_PIN  						13

#define LED_PWM_FREQ 					500
#define LED_START_PULSE_WIDTH_US 		1500

#define THRUSTER_PWM_FREQ 				500
#define THRUSTER_PWM_PERIOD				2000
#define THRUSTER_START_PULSE_WIDTH_US 	0
#define THRUSTER_MAX_PULSE_WIDTH_US 	1900
#define THRUSTER_MIN_PULSE_WIDTH_US 	1100

#define PWM_FREQ_SCALING				THRUSTER_PWM_FREQ * 3
#define LED_PWM_SCALING				LED_PWM_FREQ * 3

#define NUM_THRUSTERS 					8

enum thruster_mapping
{
	THR5,
	THR4,
	THR6,
	THR7,
	THR1,
	THR0,
	THR2,
	THR3,
}thruster_mapping;

enum sequence_type
{
	NO_SEQUENCE,
	SEQUENCE_START,
	SEQUENCE_ARM,
	SEQUENCE_DISARM,
}sequence_type;

typedef enum msg_state
{
	MSG_STATE_MAGIC_BYTES_NOT_RECEIVED,
	MSG_STATE_MAGIC_BYTES_RECEIVED,
	MSG_STATE_SEND_OK,
	MSG_STATE_SEND_FAIL,
	MSG_STATE_RECEIVE_OK,
	MSG_STATE_RECEIVE_FAIL,
}msg_state;

typedef enum msg_type
{
	MSG_TYPE_NOTYPE,
	MSG_TYPE_THRUSTER 		= 0x41,
	MSG_TYPE_LIGHT 		= 0x42,
	MSG_TYPE_HEARTBEAT		= 0x43,
	MSG_TYPE_ACK 			= 0x44,
	MSG_TYPE_NOACK 		= 0x45,
	MSG_TYPE_ARM 			= 0x46,
	MSG_TYPE_DISARM 		= 0x47,
}msg_type;

void start_sequence(void);
void arm_sequence(void);
void disarm_sequence(void);
void initLeTimer(void);
void send_vortex_msg(msg_type type);

uint32_t us_to_comparevalue(uint32_t us, TIMER_TypeDef *timer);
uint8_t update_thruster_pwm(uint8_t *pwm_data_ptr);
uint8_t update_light_pwm(uint8_t *pwm_data_ptr);
bool crc_passed(uint8_t * receive_data);

#endif // ROV_UTILITIES_H
