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

void USART1_RX_IRQHandler(void)
{
	if(USART1->STATUS & USART_STATUS_RXDATAV)
	{
		receiveBuff.data[receiveBuff.writeIndex] = USART_RxDataGet(USART1);

		//send back data for debugging
		uartPutData(&receiveBuff.data[receiveBuff.writeIndex]);

		if(receiveBuff.pendingBytes++ >= BUFFERSIZE) receiveBuff.pendingBytes = BUFFERSIZE;

		if(receiveBuff.writeIndex++ >= BUFFERSIZE-1)
		{
			receiveBuff.overflow = true;
			receiveBuff.writeIndex = 0;
		}

		//USART_IntClear(UART, USART_IF_RXDATAV);
	}
}

void uartPutData(uint8_t *data)
{
    USART_Tx(USART1, *data);
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
	USART_Reset(USART1);

	CMU->CTRL |= (1 << 14); 	                    // Set HF clock divider to /2 to keep core frequency <32MHz
	CMU->OSCENCMD |= 0x4;                           // Enable XTAL Oscillator

	while(! (CMU->STATUS & 0x8) );                  // Wait for XTAL osc to stabilize
	CMU->CMD = 0x2;                                 // Select HF XTAL osc as system clock source. 48MHz XTAL, but we divided the system clock by 2, therefore our HF clock should be 24MHz

	CMU->HFPERCLKEN0 = (1 << 13) | (1 << 1);        // Enable GPIO, and USART1 peripheral clocks

	GPIO->P[COM_PORT].MODEL = (1 << 4) | (4 << 0);  // Configure PD0 as digital output and PD1 as input
	GPIO->P[COM_PORT].DOUTSET = (1 << USART_TX_pin); // Initialize PD0 high since UART TX idles high (otherwise glitches can occur)

	USART_IntClear(USART1, _USART_IF_MASK);
	USART_IntEnable(USART1, USART_IF_RXDATAV);

	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);

	//NVIC_ClearPendingIRQ(USART1_TX_IRQn);
	//NVIC_EnableIRQ(USART1_TX_IRQn);


	// Use default value for USART1->CTRL: asynch mode, x16 OVS, lsb first, CLK idle low
	// Default frame options: 8-none-1-none
	USART1->CLKDIV = (152 << 6);                           		// 152 will give 38400 baud rate, 51 will give 115200 baud rate (using 16-bit oversampling with 24MHz peripheral clock)
	USART1->CMD = (1 << 11) | (1 << 10) | (1 << 2) | (1 << 0); 	// Clear RX/TX buffers and shif regs, Enable Transmitter and Receiver
	USART1->IFC = 0x1FF9;                                      	// clear all USART interrupt flags
	USART1->ROUTE = 0x103;                                     	// Enable TX and RX pins, use location #1 (UART TX and RX located at PD0 and PD1, see EFM32GG990 datasheet for details)

}

