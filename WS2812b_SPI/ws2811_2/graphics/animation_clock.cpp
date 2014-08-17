#include "animation_clock.h"
#include "animation_fading.h"

CAnimationClock::CAnimationClock(IPixelDisplay *display, RTC_AnimationDriver *rtc, const RGBColor hour, const RGBColor minute, const RGBColor second)
    : IAnimation(display), rtc(rtc), hourColor(hour), minuteColor(minute), secondColor(second)
{

}
void CAnimationClock::doAnimationStep(void)
{
    //should check if rtc or display are NULL pointer, but since this method is called very often I don't want to waste CPU
    static RTC_TimeTypeDef time;
    rtc->rtc_getTime(time);

    time.RTC_Hours = time.RTC_Hours %10;
    time.RTC_Minutes = time.RTC_Minutes % 10;
    time.RTC_Seconds = time.RTC_Seconds % 10;

    display->clearAllPixel();
    display->Pixel[time.RTC_Hours] = addColor(display->Pixel[time.RTC_Hours], hourColor);
    display->Pixel[time.RTC_Minutes] = addColor(display->Pixel[time.RTC_Minutes], minuteColor);
    display->Pixel[time.RTC_Seconds] = addColor(display->Pixel[time.RTC_Seconds], secondColor);

    static CAnimationFading fadeIn(display, rtc, CAnimationFading::EFadingType::FADE_IN);
    static CAnimationFading fadeOut(display, rtc, CAnimationFading::EFadingType::FADE_OUT);

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
