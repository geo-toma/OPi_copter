//#include "gpio.h"
//#include <unistd.h>

/**
 * Low level configuration of GPIO pin mode
 */
/*
#define IOMEM(x) ((void __force __iomem *)(x))
#ifndef IO_ADDRESS
#define IO_ADDRESS(x) ((x) + 0xf0000000)
#endif
#ifndef __io_address
#define __io_address(n) IOMEM(IO_ADDRESS(n))
#endif


// IO memory map base address, discovered by searching through Armbian sources
#define SUNXI_IOMEM(n) (0xf0000000 + (n))
#define PIO_BASE SUNXI_IOMEM(0x01C20800)
*/

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

using namespace std;

#define GPIO_BASE	0x01C20800
#define BLOCK_SIZE	(4*1024)

struct ALLWINNER_peripheral
{
	unsigned long phy_addr; // Physical address in memory
	int mem_fld; // System memory file descriptor (return by open /dev/mem file)
	void *map; // Function mapped physical and virtual address
	volatile unsigned int *vir_addr; // Peripheral's virtual address pointer
};

void map_peripheral(struct ALLWINNER_peripheral *p){
	
	// Open /dev/mem system memory file or /dev/gpiomem gpio memory file for security
	if ((p->mem_fld = open("/dev/mem", O_RDWR|O_SYNC)) < 0)
	{
		// Failed to open /dev/mem or /dev/gpiomem, try checking permission
		cout << "GPIO ECHEC" << endl;
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
	p->vir_addr = (volatile unsigned int*)p->map;
	
}
int main(int argc, char const *argv[])
{
	/*
		init_gpio();
		struct pin p;
		p.port = gpioPort::PA;
		p.num = 13;
		gpio_mode(p, pinMode::OUTPUT, 0);
		while(1){
			gpio_write(p, pinState::HIGH);
			usleep(1000000);
			gpio_write(p, pinState::LOW);
			usleep(1000000);
		}*/

	//void __iomem *base = __io_address(0x01c28000);
   // printk("GPIO: base                     = 0x%lx, %s \n", (long unsigned int)base, GPIO_PIN_DESC);
    //printk("GPIO: PIO_BASE                 = 0x%lx, %s \n", (long unsigned int)PIO_BASE, GPIO_PIN_DESC);
	
	struct ALLWINNER_peripheral gpio = {GPIO_BASE}; // Only the phy_addr is specify
	map_peripheral(&gpio);
	cout << *gpio.vir_addr << endl;

	return 0;
}