#ifndef ANIMATION_FADING_H_INCLUDED
#define ANIMATION_FADING_H_INCLUDED

#include <animation.h>
#include <graphics.h>
#include <RTC_AnimationDriver.h>

class CAnimationFading : public IAnimation
{
public:
    enum EFadingType{
        FADE_IN,
        FADE_OUT
    };

    RTC_AnimationDriver *rtc;
    const EFadingType fadingType;
    CAnimationFading(IPixelDisplay *display, RTC_AnimationDriver *rtc, EFadingType fadingType);
    void setuptading(uint16_t pixel, uint16_t steps);

protected:
    virtual void doAnimationStep(void);

private:
    uint16_t currentstep;
    uint16_t pixel;
    uint16_t steps;
    bool animationRegestired;
    void fadeIn();
    void fadeOut();
};

#endif /* ANIMATION_FADING_H_INCLUDED */
