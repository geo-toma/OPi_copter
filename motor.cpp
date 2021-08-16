/*** Georges N'MENA **August 2021**  ***/
/***      nmenageorges@gmail.com     ***/

/* Compilation of this code g++ motor.cpp -lpthread*/

#include <fstream>
#include <thread>

using namespace std;

#include "gpio.cpp"

#define TIMER_BASE  	0x01C20C00

#define TMR_IRQ_EN_REG 		*(timer.vir_addr + 0x0/4)
#define TMR_IRQ_STA_REG 		*(timer.vir_addr + 0x4/4)
#define TMR0_CTRL_REG 		*(timer.vir_addr + 0x10/4)
#define TMR0_INTV_VALUE_REG 		*(timer.vir_addr + 0x14/4)
#define TMR0_CUR_VALUE_REG 		*(timer.vir_addr + 0x18/4)

struct ALLWINNER_peripheral timer = {TIMER_BASE}; // Only the phy_addr is specify

int32_t cmd_motor1(0);
int32_t cmd_motor2(0);
int32_t cmd_motor3(0);
int32_t cmd_motor4(0);

struct pin m1_pin;
struct pin m2_pin;
struct pin m3_pin;
struct pin m4_pin;

void init(){
	#ifndef PWM_INIT
	map_peripheral(&timer);
	if (timer.map == MAP_FAILED) return;
	init_gpio();
	#define PWM_INIT
	// Set interval between count (7500 --> 50Hz with pre-scale = 64 and OSC24M)
	TMR0_INTV_VALUE_REG = 0x1D4C; 
	TMR_IRQ_EN_REG |= 0x1; // Enable interrupt
	TMR0_CTRL_REG &= 0xFFFFFF00; // Clear all config
	TMR0_CTRL_REG |= 0x64; // Continuous mode, 64 pre-scale, OSC24M
	TMR0_CTRL_REG |= 0x3; // Set reload and start bit

	// Set motor pin in OUTPUT Mode
	gpio_mode(m1_pin, pinMode::OUTPUT, 0);
	pull_mode(m1_pin, pullMode::NONE);
	gpio_mode(m2_pin, pinMode::OUTPUT, 0);
	pull_mode(m2_pin, pullMode::NONE);
	gpio_mode(m3_pin, pinMode::OUTPUT, 0);
	pull_mode(m3_pin, pullMode::NONE);
	gpio_mode(m4_pin, pinMode::OUTPUT, 0);
	pull_mode(m4_pin, pullMode::NONE);
	#endif
}

void runMotor(struct pin *p, int32_t *activeH){
	while(1){
		if (TMR0_CUR_VALUE_REG > *activeH)
			gpio_write(*p, pinState::LOW);
		if (TMR0_CUR_VALUE_REG < *activeH)
			gpio_write(*p, pinState::HIGH);
	}
}

void updateCmd(int32_t *new_c, int32_t *old_c){
	if(*new_c > *old_c+10 || *new_c < *old_c-10){
		*old_c = *new_c;
	}
}

int main(int argc, char const *argv[])
{
	init();

	std::thread th1(runMotor, &m1_pin, &cmd_motor1);
	std::thread th2(runMotor, &m2_pin, &cmd_motor2);
	std::thread th3(runMotor, &m3_pin, &cmd_motor3);
	std::thread th4(runMotor, &m4_pin, &cmd_motor4);

	int32_t tmp_cmd_m1 = cmd_motor1;
	int32_t tmp_cmd_m2 = cmd_motor2;
	int32_t tmp_cmd_m3 = cmd_motor3;
	int32_t tmp_cmd_m4 = cmd_motor4;

	ifstream is;

	while(1){
		is.open(".cmdMotor");
		if(is){
			is >> tmp_cmd_m1;
			is >> tmp_cmd_m2;
			is >> tmp_cmd_m3;
			is >> tmp_cmd_m4;
		}
		updateCmd(&tmp_cmd_m1, &cmd_motor1);
		updateCmd(&tmp_cmd_m2, &cmd_motor2);
		updateCmd(&tmp_cmd_m3, &cmd_motor3);
		updateCmd(&tmp_cmd_m4, &cmd_motor4);
		is.close();
	}

	th1.join();
	th2.join();
	th3.join();
	th4.join();
	return 0;
}