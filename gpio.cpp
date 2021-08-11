/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

int map_peripheral(struct gpio_pin *p){
	// Open /dev/mem system memory file or /dev/gpiomem gpio memory file for security
	if ((p->mem_fld = open("/dev/mem", O_RDWR|O_SYNC)) < 0)
	{
		// Failed to open /dev/mem or /dev/gpiomem, try checking permission
		return -1;
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
		return -1;
	}

	p->vir_addr = (volatile unsigned int*)p->map;

	return 0;
}


void unmap_peripheral(struct gpio_pin *p){
	munmap(p->map, BLOCK_SIZE);
	close(p->mem_fld);
}

#ifdef SUCCESS_GPIO_MAPPED
void gpio_mode(struct pin p, pinMode mode, int8_t alt){
	*(Pn_CFG(p.port,p.num)) &= ~(7<<(4*(p.num%8)));
	switch(mode){
		case pinMode.INPUT :
			// Nothing to do : bits already set to 000
			break;
		case pinMode.OUTPUT :
			// Set the bits to 001
			*(Pn_CFG(p.port,p.num)) |= (1<<(4*(p.num%8)));
			break;
		case pinMode.ALT :
			// Set the altenate function if the correct value of function are choosed
			*(Pn_CFG(p.port,p.num)) |= (alt < 7 && alt > 1)? (alt<<(4*(p.num%8))) : 0;
			break;
	}
}

void input_mode(struct pin p, inputMode mode){
	*(Pn_PUL(p.port, p.num)) &= ~(3<<(2*(p.num%16)));
	switch(mode){
		case inputMode.NONE :
			// Bits already set to 00
			break;
		case inputMode.PULLUP :
			// Set bits to 01
			*(Pn_PUL(p.port, p.num)) |= (1<<(2*(p.num%16)));
			break;
		case inputMode.PULLDOWN :
			// Set bits to 10
			*(Pn_PUL(p.port, p.num)) |= (2<<(2*(p.num%16)));
			break;
	}
}

void gpio_write(struct pin p, pinState state){
	if(state == pinState.LOW)
		*(Pn_DAT(p.port)) &= ~(1<<p.num);
	if(state == pinState.HIGH)
		*(Pn_DAT(p.port)) |= 1<<p.num;
}

pinState gpio_read(struct pin p){
	return (*(Pn_DAT(p.port)&(1<<p.num)))? pinState.HIGH : pinState.LOW;
}
#endif