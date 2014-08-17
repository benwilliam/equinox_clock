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
#include "animation_clock.h"
#include "WS2812SPI.h"
#include "RTC_AnimationDriver.h"
#include "animation_knightRider.h"

int main(void)
{
    SystemInit();

    CWS2812SPI ws2812(60);
    ws2812.init();

    RTC_AnimationDriver rtc(&ws2812);
    rtc.init();

    CAnimationClock clock(&ws2812, &rtc);
    rtc.registerAnimation(clock);

    //CAnimationKnightRider kr(&ws2812, BLUE);
    //rtc.registerAnimation(kr);

    while(1)
    {
        rtc.updateAnimations();
    }

}
