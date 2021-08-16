/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

#include "serial.h"
#include "gpio.h"

// Serial contructor
Serial::Serial(UART m_uart){
	uart = m_uart;
	switch (uart){
		case UART::UART1 :
			uart_p = {UART1_BASE};
			break;
		case UART::UART2 :
			uart_p = {UART2_BASE};
			break;
		case UART::UART3 :
			uart_p = {UART3_BASE};
			break;
	}
	map_peripheral(&uart_p);
	base_reg = (uart_p.map == MAP_FAILED)? 0 : uart_p.vir_addr;
}

#ifdef SUCCESS_GPIO_MAPPED
void Serial::init(bool fifo_mode, BAUD s_baud){
	fifo = fifo_mode;
	set_uartPins(&pins, uart);
	/*Orange pi PC (Allwinner H3) have all uart pin config in one port*/

	// Desable pull on uart pins
	struct pin p = pins.TX_pin; // Select the first pin
	*(Pn_PUL(p.port, p.num)) &= ~(((uart == UART::UART3)?0xF:0xFF)<<(2*(p.num%16)));
	/*
		Set all uart pin to HIGH IMPEDANCE by clearing their pull register.
		UART3 has only 2 pin (RX TX) that explain the 0xF in the instruction.
		All uart pin follow each other in this order (CTS RTS RX TX) in the 
		pull register, use that to configure pull mode in one instruction. 
	*/

	// Config pin to UART function
	switch (uart){
		case UART::UART1 :
			*(Pn_CFG(p.port, p.num)) &= ~(0xFF<<(4*(p.num%8))); // Clear function on RX and TX pins
			*(Pn_CFG(p.port, p.num)) |= 0x22<<(4*(p.num%8)); // Set uart function on RX and TX pins
			*(Pn_CFG(p.port, p.num+2)) &= ~(0xFF<<(4*((p.num+2)%8))); // Clear function on CTS and RTS pins
			*(Pn_CFG(p.port, p.num+2)) |= 0x22<<(4*((p.num+2)%8)); // Set uart function on CTS and RTS pins
			break;
		case UART::UART2 :
			*(Pn_CFG(p.port, p.num)) &= ~(0xFFFF<<(4*(p.num%8))); // Clear function on CTS, RTS, RX and TX pins
			*(Pn_CFG(p.port, p.num)) |= 0x2222<<(4*(p.num%8)); // Set uart function on CTS, RTS, RX and TX pins
			break;
		case UART::UART3 :
			*(Pn_CFG(p.port, p.num)) &= ~(0xFF<<(4*(p.num%8))); // Clear function on RX and TX pins
			*(Pn_CFG(p.port, p.num)) |= 0x33<<(4*(p.num%8)); // Set uart function on RX and TX pins
			break;
	} // UART1's pins are in two config register

	// Clear all serial interrupt (They will be set after)
	*(base_reg + UART_IER) &= 0x70;

	// Test if uart are not busy
	if (not(*(base_reg + UART_USR) & 0x1)){
		setBaud(BAUD::B_0);
		// Enable all interrupt
		*(base_reg + UART_IER) |= 0x8F;
		// Configure fifo
		*(base_reg + UART_FCR) &= ~(0xFF);
		if(fifo_mode and uart != UART::UART3)
			*(base_reg + UART_FCR) |= 0x0F;
		// Setup line control register
		*(base_reg + UART_LCR) &= ~(3<<6); // Set DLAB and BC to 0
		line_config(0x08); // Config parity, stop bit and character lenght
		// Setup Mode control register
		*(base_reg + UART_MCR) &= ~(0x3F);
		*(base_reg + UART_MCR) |= 0x23;

		// Then set the real baud rate
		setBaud(s_baud);
	}

}

void Serial::setBaud(BAUD baud){
	// Set DLAB (LCR[7]) to 1
	*(base_reg + UART_LCR) |= 1<<7;
	// Clear BaudDIV LSB and MSB
	*(base_reg + UART_DLL) &= ~(0xFF);
	*(base_reg + UART_DLH) &= ~(0XFF);
	if (baud){ // Test if baud don't equal to 0
		*(base_reg + UART_DLL) |= (baud&0xFF);
		*(base_reg + UART_DLH) |= (baud&0xFF00)>>8;
	}

	usleep(10); // Wait at least 8 cycle of the slowest uart clock

	// Clear DLAB (LCR[7])
	*(base_reg + UART_LCR) &= ~(1<<7);
}

void Serial::line_config(int8_t conf){
	if(conf < 0x3F){ // be sure that we don't touch other bit
		// Parity, Stop bit and data lenght config take only 6 bits
		*(base_reg + UART_LCR) &= ~0x3F; // Clear first the bits
		*(base_reg + UART_LCR) |= conf; // Then set them
	}
	/*
		bits 1-0 : set data lenght (character length)
		bit 2    : set number of stop bit 
		bit 3    : enable/desable parity
		bits 5-4 : set odd/even parity (bit 5 is usualy unused)
	*/
}

void Serial::close(){
	struct pin p = pins.TX_pin; // Select the first pin
	// Config uart pin to input function (default function)
	switch (uart){
		case UART::UART1 :
			*(Pn_CFG(p.port, p.num)) &= ~(0xFF<<(4*(p.num%8))); // Clear function on RX and TX pins
			*(Pn_CFG(p.port, p.num+2)) &= ~(0xFF<<(4*((p.num+2)%8))); // Clear function on CTS and RTS pins
			break;
		case UART::UART2 :
			*(Pn_CFG(p.port, p.num)) &= ~(0xFFFF<<(4*(p.num%8))); // Clear function on CTS, RTS, RX and TX pins
			break;
		case UART::UART3 :
			*(Pn_CFG(p.port, p.num)) &= ~(0xFF<<(4*(p.num%8))); // Clear function on RX and TX pins
			break;
	} // UART1's pins are in two config register
}

int8_t Serial::available(){
	if (not(*(base_reg + UART_LSR) & 0x1))
		return 0;
	else if(not(fifo))
		return 8;
	else
		return *(base_reg + UART_RFL);
}

bool Serial::write(int8_t data){
	if(
		not(*(base_reg + UART_LCR) & 0x1<<7) &&
		(*(base_reg + UART_LSR) & 0x1<<5) &&
		(not(fifo) || (*(base_reg + UART_USR) & 0x2))
	  )
	{
		*(base_reg + UART_THR) = data;
		return 1;
	} else
		return 0;
}

int8_t Serial::read(){
	receiveErr = 0;
	if (*(base_reg + UART_LSR) & 0x1){ // Test data ready
		
		if( // Test error in receive data
			(not(fifo) || (*(base_reg + UART_LSR) & 0x1<<7)) &&
			(not(*(base_reg + UART_LSR) & 0x7<<1))
		)receiveErr = 0;
		else receiveErr = 1;

		return (*(base_reg + UART_RBR) & 0xff);	
	}	
	return 0; // No data to read
}

int32_t Serial::readByte(int8_t byte){
	// The MSB of data is the first bit read from receive buffer
	int32_t data(0);
	for (int i = 0; i < byte; ++i)
	{
		if(this->available() && i < 4){
			int8_t dt = this->read();
			data = data<<8 | dt; 
		}
	}
	return data;
}
#endif
