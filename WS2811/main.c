#include "WS2811.h"

int main(void)
{
	ws2811_init();
	setAllLEDColor(0b10101010, 0b10101010, 0b10101010);
    while(1)
    {
    	start_dma();
    	startTimer();
    	setAllLEDColor(170, 170, 170);
    	volatile uint32_t result = stopTimer();


    }
}
