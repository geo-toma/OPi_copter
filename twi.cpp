/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

/*
	i2c speed will be 100kHz (can be reconfigure TWI_CCR)
*/

#include "twi.h"

Twi::Twi(TWI m_twi){
	twi = m_twi;
	switch(twi){
		case TWI::TWI0 :
			twi_p = {TWI0_BASE};
			break;
		case TWI::TWI1 :
			twi_p = {TWI1_BASE};
			break;
	}

	map_peripheral(&twi_p);
	base_reg = (twi_p.map == MAP_FAILED)? 0 : twi_p.vir_addr;
}

#ifdef SUCCESS_GPIO_MAPPED
void Twi::init(){
	if base_reg == 0 return;
	// Reset i2c soft
	*(base_reg + TWI_SRST) |= 0x1;

	set_twiPins(&pins);

	// Get the first pin (order : SCL, SDA)
	struct pin p = pins.SCL_pin;

	// Set I2C function on these pins 
	*(Pn_CFG(p.port, p.num)) &= ~(0xFF << (4*(p.num%8))); // Clear function
	switch(twi){
		case TWI::TWI0 :
			*(Pn_CFG(p.port, p.num)) |= 0x22 << (4*(p.num%8)); 
			break;
		case TWI::TWI1 :
			*(Pn_CFG(p.port, p.num)) |= 0x33 << (4*(p.num%8)); 
			break;
	}

	// Set the two pin in pull up mode
	*(Pn_PUL(p.port, p.num)) &= ~(0xF << (2*(p.num%16))); // Clear pull
	*(Pn_PUL(p.port, p.num)) |= 0x5 << (2*(p.num%16)); // Set pull on two pins

	// Config i2c 
	/*
		enable intterupts,
		enable i2c,
		enter master mode
	*/
	*(base_reg + TWI_CNTR) &= ~0xFF;
	*(base_reg + TWI_CNTR) |= ~0xF0;

	// Set i2c speed (100kHz)
	*(base_reg + TWI_CCR) &= ~0xFF;
	*(base_reg + TWI_CCR) |= ~0x5A;
}

void Twi::read(int8_t byte, char* buff_msb, int8_t slv_addr){
	if (base_reg == 0 || slv_addr%2 != 0) return ;
	*(base_reg + TWI_CNTR) |= 0x1 << 5; // Send start condition
	// Wait start condition to be transmit
	int8_t i = 0;
	while((*(base_reg + TWI_STAT)) != 0x08 and i < 50) i++;
	if(i < 50){
		*(base_reg + TWI_DATA) = (slv_addr + 1); // +1 to told slave to send data
		i = 0;
		// Wait until addr+read bit to be transmit
		while((*(base_reg + TWI_STAT)) != 0x40 and i < 50) i++;
		if(i < 50){
			i = 0;
			int8_t tmp = byte;
			while(tmp != 0 && i < 50){
				if((*(base_reg + TWI_STAT)) != 0x50){
					*(buff_msb + byte - tmp) = *(base_reg + TWI_DATA);
					tmp--;
				}else i++;
			}
		}
	}
	*(base_reg + TWI_CNTR) |= 0x1 << 4; // Send stop condition
}

bool Twi::startTransmit(int8_t slv_addr){
	if (base_reg == 0 || slv_addr%2 != 0) return 0;
	*(base_reg + TWI_CNTR) |= 0x1 << 5; // Send start condition
	// Wait start condition to be transmit
	int8_t i = 0;
	while((*(base_reg + TWI_STAT)) != 0x08 and i < 50) i++;
	if(i < 50){
		*(base_reg + TWI_DATA) = (slv_addr + 0); // +0 tell the slave to be in receive mode
		i = 0;
		// Wait until addr+write bit to be transmit
		while((*(base_reg + TWI_STAT)) != 0x18 and i < 50) i++;
		if(i < 50)
			return 1;
	}
	return 0;
}

bool Twi::write(int8_t data){
	if (base_reg == 0) return 0;
	*(base_reg + TWI_DATA) = data;
	// Wait data to be transmit and the ACK received
	int8_t i = 0;
	while((*(base_reg + TWI_STAT)) != 0x28 and i < 50) i++;
	if i < 50 return 1;
	return 0;
}

void Twi::stopTransmit(){
	*(base_reg + TWI_CNTR) |= 0x1 << 4; // Send stop condition
}
#endif 