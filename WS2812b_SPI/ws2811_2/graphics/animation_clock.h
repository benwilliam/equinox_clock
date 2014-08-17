#ifndef ANIMATION_CLOCK_H_INCLUDED
#define ANIMATION_CLOCK_H_INCLUDED
#include <animation.h>
#include <graphics.h>
#include <RTC_AnimationDriver.h>

class CAnimationClock : public IAnimation
{
public:
    RTC_AnimationDriver *rtc;
    const RGBColor hourColor;
    const RGBColor minuteColor;
    const RGBColor secondColor;
    CAnimationClock(IPixelDisplay *display, RTC_AnimationDriver *rtc, const RGBColor hour=RED, const RGBColor minute=GREEN, const RGBColor second=BLUE);

protected:
    virtual void doAnimationStep(void);

private:
    RTC_TimeTypeDef lastTime;
};

#endif /* ANIMATION_CLOCK_H_INCLUDED */
