#ifndef VORTEX_MSG_H
#define VORTEX_MSG_H

#define MAGIC_START_BYTE 	0x24
#define MAGIC_STOP_BYTE 	0x40

#define MAX_PAYLOAD_SIZE 16
#define VORTEX_MSG_MAX_SIZE 21
#define VORTEX_MSG_TYPE_INDEX 2
#define VORTEX_MSG_START_DATA_INDEX 3

struct vortex_msg
{
	uint8_t 	magic_start;				// start transmission byte
	uint8_t 	type;						// message type
	uint8_t 	payload[MAX_PAYLOAD_SIZE];	// payload
	uint8_t 	crc_byte1;         			// crc byte
	uint8_t 	crc_byte2;					// crc byte
	uint8_t		magic_stop;					// stop transmission byte
}message = { 0, 0, {0}, 0, 0, 0 };

typedef enum msg_flag
{
	MSG_STATE_MAGIC_BYTES_NOT_RECEIVED,
	MSG_STATE_MAGIC_BYTES_RECEIVED,
	MSG_STATE_SEND_OK,
	MSG_STATE_SEND_FAIL,
	MSG_STATE_RECEIVE_OK,
	MSG_STATE_RECEIVE_FAIL,
	MSG_TYPE_NOTYPE,
	MSG_TYPE_THRUSTER = 0x41,
	MSG_TYPE_LED,
	MSG_TYPE_ACK,
	MSG_TYPE_NOACK,
}msg_flag;


#endif
