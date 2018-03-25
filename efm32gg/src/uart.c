#include "uart.h"
#include "efm32gg990f1024.h"

struct circularBuffer
{
  uint8_t 	data[BUFFERSIZE];		// data buffer
  uint8_t 	readIndex;         		// read index
  uint8_t 	writeIndex;        		// write index
  uint8_t  	start_byte_index;		// start byte index
  uint8_t  	stop_byte_index;		// stop byte index
  bool		received_start_byte;	// MAGIC_START_BYTE is received
  bool		received_stop_byte;		// MAGIC_STOP_BYTE is received
} receiveBuff, transmitBuff = { {0}, 0, 0, -1, -1, false, false };

struct vortex_msg
{
	uint8_t 	magic_start;				// start transmission byte
	uint8_t 	type;						// message type
	uint8_t 	payload[MAX_PAYLOAD_SIZE];	// payload
	uint8_t 	crc_byte1;         			// crc byte
	uint8_t 	crc_byte2;					// crc byte
	uint8_t		magic_stop;					// stop transmission byte
}vortex_message = { MAGIC_START_BYTE, 0, {0}, 0, 0, MAGIC_STOP_BYTE };

void USART1_RX_IRQHandler(void)
{
	if (UART->STATUS & USART_STATUS_RXDATAV)
	{
		receiveBuff.data[receiveBuff.writeIndex] = USART_RxDataGet(UART);

		//send back data for debugging
		//USART_Tx(UART, &receiveBuff.data[receiveBuff.writeIndex]);

		if (receiveBuff.data[receiveBuff.writeIndex] == MAGIC_START_BYTE
			&& receiveBuff.received_start_byte == false)
		{
			receiveBuff.received_start_byte = true;
			receiveBuff.start_byte_index = receiveBuff.writeIndex;
		}

		if ((receiveBuff.data[receiveBuff.writeIndex] == MAGIC_STOP_BYTE)
			&& (receiveBuff.received_start_byte == true))
		{
			receiveBuff.received_stop_byte = true;
			receiveBuff.stop_byte_index = receiveBuff.writeIndex;
		}

		if ((receiveBuff.received_start_byte == true)
			&& 	(receiveBuff.writeIndex < receiveBuff.start_byte_index))
		{
			if (((receiveBuff.writeIndex + BUFFERSIZE) - receiveBuff.start_byte_index) > VORTEX_MSG_MAX_SIZE)
			{
				receiveBuff.received_start_byte = false;
				receiveBuff.received_stop_byte = false;
			}
		}

		if (++receiveBuff.writeIndex >= BUFFERSIZE)
		{
			receiveBuff.writeIndex = 0;
		}

		USART_IntClear(UART, USART_IF_RXDATAV);
	}
}

void initUart(void)
{
	USART_Reset(UART);

	CMU_ClockDivSet(cmuClock_HF, cmuClkDiv_2);

	// Start HFXO and wait until it is stable
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

	 // Select HFXO as clock source for HFCLK
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

	// Enable clock for USART module
	CMU_ClockEnable(cmuClock_USART1, true);

	// Enable clock for GPIO module (required for pin configuration)
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure GPIO pins
	GPIO_PinModeSet(UART_PORT, UART_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_PORT, UART_RX_PIN, gpioModeInput, 0);

	// Prepare struct for initializing UART in asynchronous mode
	USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

	uartInit.enable       = usartDisable;  	// Don't enable UART upon intialization
	uartInit.refFreq      = 0;              // Provide information on reference frequency. When set to 0, the reference frequency is
	uartInit.baudrate     = 115200;        	// Baud rate
	uartInit.oversampling = usartOVS16;     // Oversampling. Range is 4x, 6x, 8x or 16x
	uartInit.databits     = usartDatabits8; // Number of data bits. Range is 4 to 10
	uartInit.parity       = usartNoParity;  // Parity mode
	uartInit.stopbits     = usartStopbits1; // Number of stop bits. Range is 0 to 2
	uartInit.mvdis        = false;          // Disable majority voting
	uartInit.prsRxEnable  = false;          // Enable USART Rx via Peripheral Reflex System
	uartInit.prsRxCh      = usartPrsRxCh0;  // Select PRS channel if enabled

	// Initialize USART with uartInit struct
	USART_InitAsync(UART, &uartInit);

	// Prepare UART Rx and Tx interrupts
	USART_IntClear(UART, _UART_IF_MASK);
	USART_IntEnable(UART, UART_IF_RXDATAV);

	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_ClearPendingIRQ(USART1_TX_IRQn);

	NVIC_EnableIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_TX_IRQn);

	// Enable UART RX/TX PINS on UART_LOCATION
	UART->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_LOCATION;

	// Enable UART
	USART_Enable(UART, usartEnable);
}

uint8_t magic_bytes_received(void)
{
	if (receiveBuff.received_start_byte && receiveBuff.received_stop_byte)
	{
		return MSG_STATE_MAGIC_BYTES_RECEIVED;
	}
	else
	{
		return MSG_STATE_MAGIC_BYTES_NOT_RECEIVED;
	}
}

uint8_t receive_vortex_msg(uint8_t *receive_data_ptr)
{
	if (magic_bytes_received() == MSG_STATE_MAGIC_BYTES_NOT_RECEIVED)
	{
		return MSG_STATE_RECEIVE_FAIL;
	}

	int i;
	uint8_t start_index = receiveBuff.start_byte_index;
	uint8_t stop_index = receiveBuff.stop_byte_index;

	if ((start_index < 0) || (stop_index < 0) || (start_index == stop_index))
	{
		receiveBuff.received_start_byte = false;
		receiveBuff.received_stop_byte = false;
		return MSG_STATE_RECEIVE_FAIL;
	}

	if(start_index > stop_index)
	{
		*receive_data_ptr = (stop_index + BUFFERSIZE + 1) - start_index;
		receive_data_ptr++;
		for (i = start_index; i < BUFFERSIZE; i++)
		{
			*receive_data_ptr = receiveBuff.data[i];
			receive_data_ptr++;
		}

		for (i = 0; i <= stop_index; i++)
		{
			*receive_data_ptr = receiveBuff.data[i];
			receive_data_ptr++;
		}
	}
	else
	{
		*receive_data_ptr = stop_index - start_index + 1;
		receive_data_ptr++;
		for (i = start_index; i <= stop_index; i++)
		{
			*receive_data_ptr = receiveBuff.data[i];
			receive_data_ptr++;
		}
	}

	receiveBuff.received_start_byte = false;
	receiveBuff.received_stop_byte = false;

	return MSG_STATE_RECEIVE_OK;
}

void send_vortex_msg(msg_type type)
{
	switch(type)
	{
		case MSG_TYPE_NOACK:
			vortex_message.type = MSG_TYPE_NOACK;
			strcpy(&vortex_message.payload[0], "NO ACK\n");
			break;
		case MSG_TYPE_ACK:
			vortex_message.type = MSG_TYPE_ACK;
			strcpy(&vortex_message.payload[0], "ACK!\n");
			break;
		default:
			vortex_message.type = MSG_TYPE_NOTYPE;
			break;
	}

	USART_Tx(UART, vortex_message.magic_start);
	USART_Tx(UART, vortex_message.type);

	int i;
	for (i = 0; i < MAX_PAYLOAD_SIZE; i++)
	{
		USART_Tx(UART, vortex_message.payload[i]);
	}

	USART_Tx(UART, vortex_message.magic_stop);

}


