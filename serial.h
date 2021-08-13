/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

#ifndef SERIAL_H
#define SERIAL_H

#include "gpio.h"

// Define base address of uart use on Orange Pi PC
#define UART1_BASE  	0x01C28400
#define UART2_BASE  	0x01C28800
#define UART3_BASE  	0x01C28C00

// Define register offset (they are divided by 4 because they will be used in pointers)
#define UART_RBR  	0x00/4
#define UART_THR  	0x00/4
#define UART_DLL  	0x00/4
#define UART_DLH  	0x04/4
#define UART_IER  	0x04/4
#define UART_FCR  	0x08/4
#define UART_LCR  	0x0C/4
#define UART_MCR  	0x10/4
#define UART_LSR  	0x14/4
#define UART_USR  	0x7C/4
#define UART_TFL  	0x80/4
#define UART_RFL  	0x84/4


extern struct ALLWINNER_peripheral gpio; // The same gpio declared in gpio.h

enum UART
{
	UART1,UART2,UART3
};
enum BAUD
{
	// Serial clock default frequency : 24 Mhz
	// Baud divisor = (sclk)/(16*baud)
	B_0 = 0,
	B_2400 = 625,
	B_4800 = 313,
	B_9600 = 156,
	B_14400 = 104,
	B_19200 = 78,
	B_38400 = 39,
	B_57600 = 26,
	B_115200 = 13
};

struct uartPins
{
	struct pin RX_pin;
	struct pin TX_pin;
	struct pin RTS_pin;
	struct pin CTS_pin;
};

class Serial
{
	private : 
		UART uart; // Select the serial port for the communication
		struct ALLWINNER_peripheral uart_p; // Mapped uart resgister to virtual variable
		volatile unsigned int *base_reg; // Get the selected uart port base address
		bool fifo;
		bool receiveErr;

		void setBaud(BAUD baud);

	public :
		Serial(UART m_uart);

		void init(bool fifo_mode, BAUD s_baud);
		void line_config(int8_t conf);
		int8_t available();
		bool write(int8_t data);
		int8_t read();
		int32_t readByte(int8_t byte);
		void close();

		bool getReceiveErr(){ // Clear receiveErr once readed
			if(receiveErr){
				receiveErr = FALSE;
				return TRUE;
			}
			return FALSE;
		}

};

// Set the pins on which the communication will be establish
void set_uartPins(uartPins *pins, UART uart){
	switch(uart){
		case UART::UART1 :
			(pins->TX_pin).port = gpioPort::PG;
			(pins->TX_pin).num = 6;
			(pins->RX_pin).port = gpioPort::PG;
			(pins->RX_pin).num = 7;
			(pins->RTS_pin).port = gpioPort::PG;
			(pins->RTS_pin).num = 8;
			(pins->CTS_pin).port = gpioPort::PG;
			(pins->CTS_pin).num = 9;
			break;
		case UART::UART2 :
			(pins->TX_pin).port = gpioPort::PA;
			(pins->TX_pin).num = 0;
			(pins->RX_pin).port = gpioPort::PA;
			(pins->RX_pin).num = 1;
			(pins->RTS_pin).port = gpioPort::PA;
			(pins->RTS_pin).num = 2;
			(pins->CTS_pin).port = gpioPort::PA;
			(pins->CTS_pin).num = 3;
			break;
		case UART::UART3 :
			(pins->TX_pin).port = gpioPort::PA;
			(pins->TX_pin).num = 13;
			(pins->RX_pin).port = gpioPort::PA;
			(pins->RX_pin).num = 14;
			break;
	}
}

#endif