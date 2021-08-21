#include "gpio.h"
#include <unistd.h>

int main(int argc, char const *argv[])
{
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
	}
	return 0;
}