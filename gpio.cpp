/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

#include <stdlib.h>

#include "gpio.h"

// GPIO global variable
struct ALLWINNER_peripheral gpio = {GPIO_BASE}; // Only the phy_addr is specify


void map_peripheral(struct ALLWINNER_peripheral *p){
	/*
		// Open /dev/mem system memory file or /dev/gpiomem gpio memory file for security
		if ((p->mem_fld = open("/dev/mem", O_RDWR|O_SYNC)) < 0)
		{
			// Failed to open /dev/mem or /dev/gpiomem, try checking permission
			return;
		}
		p->map = mmap(
			NULL,
			BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			p->mem_fld,
			p->phy_addr
			);

		if(p->map == MAP_FAILED)
		{
			perror("mmap");
			return;
		}
		p->vir_addr = (volatile unsigned int*)p->map;*/
	
}


void unmap_peripheral(struct ALLWINNER_peripheral *p){
	munmap(p->map, BLOCK_SIZE);
	close(p->mem_fld);
	#ifdef SUCCESS_GPIO_MAPPED
	#undef SUCCESS_GPIO_MAPPED
	#endif
}

// Try to map virtual pointer variable on physical register address
void init_gpio(){
	map_peripheral(&gpio);
	if(gpio.map == MAP_FAILED) return;
	#ifndef SUCCESS_GPIO_MAPPED
	#define SUCCESS_GPIO_MAPPED
	#endif
}

#ifdef SUCCESS_GPIO_MAPPED
void gpio_mode(struct pin p, pinMode mode, int8_t alt){
	*(Pn_CFG(p.port,p.num)) &= ~(7<<(4*(p.num%8)));
	switch(mode){
		case pinMode::INPUT :
			// Nothing to do : bits already set to 000
			break;
		case pinMode::OUTPUT :
			// Set the bits to 001
			*(Pn_CFG(p.port,p.num)) |= (1<<(4*(p.num%8)));
			break;
		case pinMode::ALT :
			// Set the altenate function if the correct value of function are choosed
			*(Pn_CFG(p.port,p.num)) |= (alt < 7 && alt > 1)? (alt<<(4*(p.num%8))) : 0;
			break;
	}
}

void pull_mode(struct pin p, pullMode mode){
	*(Pn_PUL(p.port, p.num)) &= ~(3<<(2*(p.num%16)));
	switch(mode){
		case pullMode::NONE :
			// Bits already set to 00
			break;
		case pullMode::PULLUP :
			// Set bits to 01
			*(Pn_PUL(p.port, p.num)) |= (1<<(2*(p.num%16)));
			break;
		case pullMode::PULLDOWN :
			// Set bits to 10
			*(Pn_PUL(p.port, p.num)) |= (2<<(2*(p.num%16)));
			break;
	}
}

void gpio_write(struct pin p, pinState state){
	if(state == pinState::LOW)
		*(Pn_DAT(p.port)) &= ~(1<<p.num);
	if(state == pinState::HIGH)
		*(Pn_DAT(p.port)) |= 1<<p.num;
}

pinState gpio_read(struct pin p){
	return ((*(Pn_DAT(p.port))&(1<<p.num)))? pinState::HIGH : pinState::LOW;
}
#endif