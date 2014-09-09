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

#include "stm32f10x.h"
#include "animation_clock.h"
#include "WS2812SPI.h"
#include "TIM_AnimationDriver.h"
#include "animation_knightRider.h"
#include "rtc.h"
#include <stdio.h>
int main(void)
{
   SystemInit();

     // GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOC, GPIO_Pin_13);

    RTC_Driver::init();

    CWS2812SPI ws2812(10);
    ws2812.init();

    TIM_AnimationDriver aniDriver(&ws2812);
    aniDriver.init();

    CAnimationClock clock(&ws2812, &aniDriver);
    //aniDriver.registerAnimation(clock);
    ws2812.setAllPixel(WHITE);
    ws2812.update();

    //CAnimationKnightRider kr(&ws2812, BLUE);
    //aniDriver.registerAnimation(kr);

    //CDCF77Decoder dcf77;
    //dcf77.init();

    while(1)
    {
        aniDriver.updateAnimations();
    }
}
