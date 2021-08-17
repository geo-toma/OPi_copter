/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

/*
	This i2c is to control or get information from IMU that work as slave only
	then this code will implement only master part of OPi.
	We will use 7 bits address. No need to implement 10 bits address
*/
#ifndef TWI_H
#define TWI_H

#include "gpio.h"

// Define I2C base address
#define TWI0_BASE		0x01C2AC00 	
#define TWI1_BASE		0x01C2B000 	

// Define register offset
#define TWI_ADDR		0x00/4
#define TWI_DATA		0x08/4
#define TWI_CNTR		0x0C/4
#define TWI_STAT		0x10/4
#define TWI_CCR			0x14/4
#define TWI_SRST		0x18/4

extern struct ALLWINNER_peripheral gpio; // Point to the gpio of gpio code

enum TWI
{
	TWI0, TWI1
};

struct twiPins
{
	struct pin SDA_pin;
	struct pin SCL_pin;
};

class Twi
{
private :
	TWI twi; // Allow select I2C pin and know which i2c is choosed
	struct ALLWINNER_peripheral twi_p;
	volatile unsigned int *base_reg; // Get the selected uart port base address

	struct twiPins pins;

public:
	Twi(TWI m_twi);

	void init();
	void read(int8_t byte, char* buff_msb, int8_t slv_addr);
	bool startTransmit(int8_t slv_addr);
	bool write(int8_t data);
	void stopTransmit();
	
};

// Set the pins on which the communication will be establish
void set_twiPins(twiPins *pins, TWI twi){
	switch(twi){
		case TWI::TWI0 :
			(pins->SCL_pin).port = gpioPort::PA;
			(pins->SCL_pin).num = 11;
			(pins->SDA_pin).port = gpioPort::PA;
			(pins->SDA_pin).num = 12;
			break;
		case TWI::TWI1 :
			(pins->SCL_pin).port = gpioPort::PA;
			(pins->SCL_pin).num = 18;
			(pins->SDA_pin).port = gpioPort::PA;
			(pins->SDA_pin).num = 19;
			break;
	}
}

#endif