#include "animation_clock.h"
#include "animation_fading.h"

CAnimationClock::CAnimationClock(IPixelDisplay *display, TIM_AnimationDriver *aniDriver, const RGBColor hour, const RGBColor minute, const RGBColor second)
    : IAnimation(display), aniDriver(aniDriver), hourColor(hour), minuteColor(minute), secondColor(second)
{

}
void CAnimationClock::doAnimationStep(void)
{
    //should check if aniDriver or display are NULL pointer, but since this method is called very often I don't want to waste CPU
    static RTC_TimeTypeDef time;

    if (RTC_Driver::IsSecondPast){
        RTC_Driver::IsSecondPast = false;
        RTC_Driver::rtc_getTime(time);
    }

    time.RTC_Hours = (time.RTC_Hours+6) % 12;
    time.RTC_Minutes = (time.RTC_Minutes+30)%60;
    time.RTC_Seconds = (time.RTC_Seconds+30)%60;

    display->clearAllPixel();
    display->Pixel[time.RTC_Hours*5+time.RTC_Minutes/12] = addColor(display->Pixel[time.RTC_Hours], hourColor);
    display->Pixel[time.RTC_Minutes] = addColor(display->Pixel[time.RTC_Minutes], minuteColor);
    display->Pixel[time.RTC_Seconds] = addColor(display->Pixel[time.RTC_Seconds], secondColor);

    static CAnimationFading fadeIn(display, aniDriver, CAnimationFading::EFadingType::FADE_IN);
    static CAnimationFading fadeOut(display, aniDriver, CAnimationFading::EFadingType::FADE_OUT);

    if(time.RTC_Seconds != lastTime.RTC_Seconds){
        fadeIn.setuptading(time.RTC_Seconds, 32);
        if(time.RTC_Seconds == 0){
            fadeOut.setuptading(9, 32);
        }
        else{
            fadeOut.setuptading(time.RTC_Seconds-1, 32);
        }
    }
    fadeIn.perofrmAnimationStep();
    fadeOut.perofrmAnimationStep();
    lastTime = time;
}
