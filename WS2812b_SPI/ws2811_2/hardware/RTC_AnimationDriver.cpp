#include <RTC_AnimationDriver.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_pwr.h"

std::vector<RTC_AnimationDriver *> RTC_AnimationDriver::rtcAnimationDrivers;


RTC_AnimationDriver::RTC_AnimationDriver(IPixelDisplay *display) : IAnimationDriver(display)
{

}

void RTC_AnimationDriver::rtc_InitWakeUpInterrupt(uint8_t wakeUpCounter)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    // NVIC init
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // ext Interrupt 22 einstellen (fuer WakeUp)
    EXTI_ClearITPendingBit(EXTI_Line22);
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    PWR_BackupAccessCmd(ENABLE);

    // zum einstellen muss Wakup disabled sein
    RTC_WakeUpCmd(DISABLE);

    // Teiler 16 => 32,768kHz:16 => 2048 Hz
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
    // WakeUp Counter einstellen
    RTC_SetWakeUpCounter(wakeUpCounter); //set to 16 -> 128 interrupts per second (32768Hz / Div16 = 2048; 2048 / 128 = 16

    // enable Interrupt
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    //register myself for the interrupt
    rtcAnimationDrivers.push_back(this);

    // enable Wakeup
    RTC_WakeUpCmd(ENABLE);
}



void RTC_AnimationDriver::init()
{
    RTC_InitTypeDef RTC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);


    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F; // prescaler set to optain a 1 Hz signal of the 32kHz LSI
    RTC_InitStructure.RTC_SynchPrediv = 0xFF; // prescaler set to optain a 1 Hz signal of the 32kHz LSI
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
        return;
    }


    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
    RTC_SetWakeUpCounter(16);

    /* Enable the Wakeup Interrupt */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    /* Connect EXTI_Line22 to the RTC Wakeup event */
    EXTI_ClearITPendingBit(EXTI_Line22);
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Wakeup Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //register myself for the interrupt
    rtcAnimationDrivers.push_back(this);

    /* Enable Wakeup Counter */
    RTC_WakeUpCmd(ENABLE);
}

bool RTC_AnimationDriver::rtc_getTime(RTC_TimeTypeDef &time)
{
    RTC_GetTime(RTC_Format_BIN, &time);
    //no checking for valid time yet, maybe in the future
    return true;
}

bool RTC_AnimationDriver::rtc_getDate(RTC_DateTypeDef &date)
{
    RTC_GetDate(RTC_Format_BIN, &date);
    //no checking for valid date yet, maybe in the future
    return true;
}

extern "C" {
    void RTC_WKUP_IRQHandler(void)
    {
        if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
        {
            //c++11 way of doing it
            // perform one step more of animation
//        for (RTC_AnimationDriver *driver : RTC_AnimationDriver::rtcAnimationDrivers){
//            driver->handleAnimationISR();
//        }
            for(std::vector<int>::size_type i = 0; i != RTC_AnimationDriver::rtcAnimationDrivers.size(); i++)
            {

                RTC_AnimationDriver::rtcAnimationDrivers[i]->handleAnimationISR();

            }
            // Interrupt Flags loeschen
            RTC_ClearITPendingBit(RTC_IT_WUT);
            EXTI_ClearITPendingBit(EXTI_Line22);
        }
    }
}

