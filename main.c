#include "sdk_project_config.h"
#include "ultrasonic.h"
#include "task.h"

#define LED0_PORT PTE
#define LED0_PIN  21
#define LED1_PORT PTE
#define LED1_PIN  22

void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while(cycles--);
}

int main(void)
{

	board_init();
	SRV_voidRTOSMain();


	while(1){}

	return 0;
}

