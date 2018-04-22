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

#define MAGIC_START_BYTE 				0x24
#define MAGIC_STOP_BYTE 				0x40

#define MAX_PAYLOAD_SIZE 				16
#define VORTEX_MSG_MAX_SIZE 			21
#define VORTEX_MSG_TYPE_INDEX 			2
#define VORTEX_MSG_START_DATA_INDEX 	3
#define VORTEX_MSG_CRC_BYTE_INDEX 		19

#define LETIMER_MS 						100		// how often LETIMER0_IRQHandler triggers in milliseconds
#define ARM_SEQUENCE_DURATION_MS 		2000
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
#define LED_PWM_SCALING					LED_PWM_FREQ * 3

#define NUM_THRUSTERS 					8

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
	MSG_TYPE_THRUSTER 	= 0x41,
	MSG_TYPE_LED 		= 0x42,
	MSG_TYPE_HEARTBEAT 	= 0x43,
	MSG_TYPE_ACK 		= 0x44,
	MSG_TYPE_NOACK 		= 0x45,
	MSG_TYPE_ARM 		= 0x46,
	MSG_TYPE_DISARM 	= 0x47,
}msg_type;


void start_sequence(void);
void arm_sequence(void);
void disarm_sequence(void);
void initLeTimer(void);
void send_vortex_msg(msg_type type);

uint32_t us_to_comparevalue(uint32_t us, TIMER_TypeDef *timer);
uint8_t update_thruster_pwm(uint8_t *pwm_data_ptr);
uint8_t update_led_pwm(uint8_t *pwm_data_ptr);
bool crc_passed(uint8_t * receive_data);

#endif // ROV_UTILITIES_H
