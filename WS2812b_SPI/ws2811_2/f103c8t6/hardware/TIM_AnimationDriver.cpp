#include <TIM_AnimationDriver.h>
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

std::vector<TIM_AnimationDriver *> TIM_AnimationDriver::animationDrivers;


TIM_AnimationDriver::TIM_AnimationDriver(IPixelDisplay *display) : IAnimationDriver(display)
{

}


void TIM_AnimationDriver::init()
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* Setup Timer 2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseStructure.TIM_Period =   10; // count to 10 -> every 10 ms -> 100 frames/s
   TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/2) / 1000) -1; //ms
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


    /* Enable the RTC Wakeup Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //register myself for the interrupt
    animationDrivers.push_back(this);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}


extern "C" {
    void TIM2_IRQHandler(void)
    {
        if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
        {
            //c++11 way of doing it
            // perform one step more of animation
        for (TIM_AnimationDriver *driver : TIM_AnimationDriver::animationDrivers){
            driver->handleAnimationISR();
        }
//            for(std::vector<int>::size_type i = 0; i != TIM_AnimationDriver::animationDrivers.size(); i++)
//            {
//
//                TIM_AnimationDriver::animationDrivers[i]->handleAnimationISR();
//
//            }
            // Interrupt Flags loeschen
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        }
    }
}

