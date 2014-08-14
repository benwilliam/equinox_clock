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
#include "RTC.h"
#include "graphics.h"

void delay_ms(uint32_t time){
    time = time*100000;
    for(uint32_t c = 0; c <= time; c++);
}
int main(void)
{
    SystemInit();
    ws2811_init();
    rtc_init();

    RTC_TimeTypeDef time;

    color defaultColor = BLACK;

    color secondColor = BLUE;
    color minuteColor = GREEN;
    color hourColor = RED;

    while(1)
    {
        setAllLED_Color(&defaultColor);
        rtc_getTime(&time);
        uint8_t lastSecond=  time.RTC_Seconds;
        uint8_t secondLed = time.RTC_Seconds & 10 ;
        addColorToLED(secondLed, &secondColor);

//        uint8_t minuteLed = time.RTC_Minutes;
//        addColorToLED(minuteLed, &minuteColor);
//
//        uint8_t hourLed = time.RTC_Hours*5;
//        addColorToLED(hourLed, &hourColor);


        startFadingLED(secondLed,1,100, 64);
        while(time.RTC_Seconds == lastSecond){
                rtc_getTime(&time);

        }


//        for(uint8_t i = 0; i<=100; i++)
//        {
//            setAllLED_Color(&defaultColor);
//            //setBrightnessOfALL(i);
//            updateLED();
//            delay_ms(10);
//        }

    }
}
