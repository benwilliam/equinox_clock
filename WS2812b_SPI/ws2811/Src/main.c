/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "WS2811.h"

void delay_ms(uint32_t time){
    time = time*100000;
    for(uint32_t c = 0; c <= time; c++);
}
int main(void)
{
    SystemInit();
    ws2811_init();
    setAllLED(255, 255, 255);
    while(1)
    {

        updateLED();
        delay_ms(1000);

        for(uint8_t r = 1; r!=0; r++){
            setAllLED(r, 0, 0);
             updateLED();
             delay_ms(100);
        }

         for(uint8_t g = 1; g!=0; g++){
            setAllLED(0, g, 0);
             updateLED();
             delay_ms(100);
        }

         for(uint8_t b = 1; b!=0; b++){
            setAllLED(b, 0, 0);
             updateLED();
             delay_ms(100);
        }
    }
}
