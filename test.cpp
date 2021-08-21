//#include "gpio.h"
//#include <unistd.h>

/**
 * Low level configuration of GPIO pin mode
 */
#ifdef KERNEL_34
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

	void __iomem *base = __io_address(0x01c28000);
    printk("GPIO: base                     = 0x%lx, %s \n", (long unsigned int)base, GPIO_PIN_DESC);
    printk("GPIO: PIO_BASE                 = 0x%lx, %s \n", (long unsigned int)PIO_BASE, GPIO_PIN_DESC);
	return 0;
}