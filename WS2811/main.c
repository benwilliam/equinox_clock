#include "WS2811.h"

int main(void)
{
	ws2811_init();
	setAllLEDColor(0b10101010, 0b10101010, 0b10101010);
	setAllLEDColor(170, 170, 170);
    while(1)
    {
    	start_dma();
    	startTimer();
    	LED_TO_PWM();
    	volatile uint32_t result = stopTimer();


    }
}
