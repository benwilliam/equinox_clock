#include "WS2811.h"

int main(void)
{
	ws2811_init();
	setAllLEDColor(0b10101010, 0b10101010, 0b10101010);
    while(1)
    {
    	setAllLEDColor(170, 170, 170);

    	start_dma();
    }
}
