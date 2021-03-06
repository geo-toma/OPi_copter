/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define GPIO_BASE	0x01C20800
#define BLOCK_SIZE	(4*1024)

struct ALLWINNER_peripheral
{
	unsigned long phy_addr; // Physical address in memory
	//int mem_fld; // System memory file descriptor (return by open /dev/mem file)
	//void *map; // Function mapped physical and virtual address
	volatile unsigned int *vir_addr; // Peripheral's virtual address pointer
};

enum pullMode
{
	PULLUP,
	PULLDOWN,
	NONE // HIGH_IMPEDANCE
};
enum pinMode
{
	INPUT,OUTPUT,ALT
};
enum pinState
{
	LOW,HIGH
};
enum gpioPort
{
	PA,PC,PD,PG
};

// Define GPIO configure register pointer (there are 7 port in general OPi only use 4)
#define Pn_CFG(n,x)		(gpio.vir_addr + (n*0x24 + 0x00)/4 + x/8)
// Define INPUT mode (pull up/down) register pointer
#define Pn_PUL(n,x)		(gpio.vir_addr + (n*0x24 + 0x1C)/4 + x/16) 
// Define DATA register pointer
#define Pn_DAT(n)		(gpio.vir_addr + (n*0x24 + 0x10)/4)
/*
	PA : n = 0
	PD : n = 1
	PC : n = 2
	PG : n = 3
	x is the number of the gpio in the port
*/


struct pin
{
	gpioPort port;
	int8_t num;	 // Number of the gpio in the port
};

// Map and Unmap function
void map_peripheral(struct ALLWINNER_peripheral *p); // map the physical address to virtual address
void unmap_peripheral(struct ALLWINNER_peripheral *p);
// Try to map virtual pointer variable on physical register address
void init_gpio();
void gpio_mode(struct pin p, pinMode mode, int8_t alt);
void pull_mode(struct pin p, pullMode mode);
void gpio_write(struct pin p, pinState state);
pinState gpio_read(struct pin p);

#endif