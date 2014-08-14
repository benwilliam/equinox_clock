#ifndef RTC_H
#define RTC_H

#include "stm32f4xx_rtc.h"

void rtc_init();
void rtc_getTime(RTC_TimeTypeDef *time);
void rtc_getDate(RTC_DateTypeDef *date);

void rtc_InitWakeUpInterrupt();
void setAnimationCallback(void(*aniStep)(void));

#endif RTC_H
