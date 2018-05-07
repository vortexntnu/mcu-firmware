#include "uart.h"

struct circularBuffer
{
	volatile uint8_t 	data[BUFFERSIZE];		// data buffer
	volatile uint8_t 	readIndex;				// read index
	volatile uint8_t 	writeIndex;				// write index
	volatile uint8_t  	start_byte_index;		// start byte index
	volatile uint8_t  	stop_byte_index;		// stop byte index
	volatile bool		received_start_byte;	// MAGIC_START_BYTE is received
	volatile bool		received_stop_byte;		// MAGIC_STOP_BYTE is received
} receiveBuff, transmitBuff = { {0}, 0, 0, 0, 0, false, false };

volatile int16_t start_stop_byte_diff = 0;


void USART1_RX_IRQHandler(void)
{
	if (UART->STATUS & USART_STATUS_RXDATAV)
	{
		// write data to receive buffer
		receiveBuff.data[receiveBuff.writeIndex] = USART_RxDataGet(UART);

		// receive MAGIC_START_BYTE
		// store the index where the byte is written
		if (receiveBuff.data[receiveBuff.writeIndex] == MAGIC_START_BYTE
			&& receiveBuff.received_start_byte == false)
		{
			receiveBuff.received_start_byte = true;
			receiveBuff.start_byte_index = receiveBuff.writeIndex;
		}

		// difference in index between start and stop byte
		start_stop_byte_diff = receiveBuff.writeIndex - receiveBuff.start_byte_index;

		// receive MAGIC_STOP_BYTE
		if ((receiveBuff.data[receiveBuff.writeIndex] == MAGIC_STOP_BYTE)
			&& (receiveBuff.received_start_byte == true))
		{
			if (start_stop_byte_diff < 0)
			{
				start_stop_byte_diff = (receiveBuff.writeIndex + BUFFERSIZE) - receiveBuff.start_byte_index;
			}
			else
			{
				start_stop_byte_diff = receiveBuff.writeIndex - receiveBuff.start_byte_index;
			}

			// check to make sure STOP_BYTE is not a part of the data payload
			if ((   start_stop_byte_diff == TYPE_ONLY_MSG_SIZE - 1)
				|| (start_stop_byte_diff == (VORTEX_MSG_MAX_SIZE -1))
			    || ((start_stop_byte_diff == LIGHT_MSG_SIZE-1)
			    && ((receiveBuff.data[receiveBuff.start_byte_index + 1]) ==  MSG_TYPE_LIGHT)))
			{
				receiveBuff.received_stop_byte = true;
				receiveBuff.stop_byte_index = receiveBuff.writeIndex;
			}
		}

		// check if end of receive buffer is reached
		if ((receiveBuff.received_start_byte == true)
			&& 	(receiveBuff.writeIndex < receiveBuff.start_byte_index))
		{
			// reset start/stop byte index if message size is too big
			if (((receiveBuff.writeIndex + BUFFERSIZE) - receiveBuff.start_byte_index) > VORTEX_MSG_MAX_SIZE)
			{
				receiveBuff.received_start_byte = false;
				receiveBuff.received_stop_byte = false;
			}
		}

		receiveBuff.writeIndex++;

		if (receiveBuff.writeIndex >= BUFFERSIZE)
		{
			receiveBuff.writeIndex = 0;
		}

		USART_IntClear(UART, USART_IF_RXDATAV);
	}
}


void initUart(void)
{
	USART_Reset(UART);

	// Configure GPIO pins
	GPIO_PinModeSet(UART_PORT, UART_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_PORT, UART_RX_PIN, gpioModeInput, 0);

	// Prepare struct for initializing UART in asynchronous mode
	USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

	uartInit.enable       = usartDisable;  	// Don't enable UART upon intialization
	uartInit.refFreq      = 0; 				// Provide information on reference frequency
	uartInit.baudrate     = 115200;        	// Baud rate
	uartInit.oversampling = usartOVS4;      // Oversampling. Range is 4x, 6x, 8x or 16x
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

	// check if circular buffer is written "around"
	if (start_index > stop_index)
	{
		// store message length
		*receive_data_ptr = (stop_index + BUFFERSIZE + 1) - start_index;
		receive_data_ptr++;

		// write data from receive buffer to receive data
		// from start_index to the end of buffer
		for (i = start_index; i < BUFFERSIZE; i++)
		{
			*receive_data_ptr = receiveBuff.data[i];
			receive_data_ptr++;
		}

		// write the rest of the data
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

		// write data from receive buffer to receive data
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


void USART_PutData(uint8_t *data_ptr, uint8_t size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		USART_Tx(UART, *data_ptr);
		data_ptr++;
	}
}
