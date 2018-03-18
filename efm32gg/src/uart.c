#include "uart.h"
#include "efm32gg990f1024.h"

volatile struct circularBuffer
{
  uint8_t  data[BUFFERSIZE];	// data buffer
  uint16_t readIndex;         	// read index
  uint16_t writeIndex;        	// write index
  uint16_t pendingBytes;		// number of unread bytes
  bool     overflow;          	// buffer overflow indicator
}receiveBuff, transmitBuff = { {0}, 0, 0, 0, false };

volatile struct dummyStruct
{
	uint8_t 	magic;				// start transmission byte
	uint8_t 	type;				// message type
	uint8_t 	payload[BUFFERSIZE];// payload
	uint16_t 	crc_bytes;         	// crc bytes
	uint16_t 	writeIndex;        	// write index
	uint16_t 	pendingBytes;		// number of unread bytes
	bool     	overflow;          	// buffer overflow indicator
};

void USART1_RX_IRQHandler(void)
{
	if(UART->STATUS & USART_STATUS_RXDATAV)
	{
		receiveBuff.data[receiveBuff.writeIndex] = USART_RxDataGet(UART);

		//send back data for debugging
		uartPutData(&receiveBuff.data[receiveBuff.writeIndex]);

		if(++receiveBuff.pendingBytes >= BUFFERSIZE) receiveBuff.pendingBytes = BUFFERSIZE;

		if(++receiveBuff.writeIndex >= BUFFERSIZE-1)
		{
			receiveBuff.overflow = true;
			receiveBuff.writeIndex = 0;
		}

		USART_IntClear(UART, USART_IF_RXDATAV);
	}
}

void uartPutData(uint8_t *data)
{
    USART_Tx(UART, *data);
}

void uartGetData(uint8_t *data)
{
  if(receiveBuff.pendingBytes > 0)
  {
		*data = receiveBuff.data[receiveBuff.readIndex];
		if(receiveBuff.readIndex++ >= (BUFFERSIZE)) receiveBuff.readIndex = 0;
		if(receiveBuff.pendingBytes-- <= 0) receiveBuff.pendingBytes = 0;
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
