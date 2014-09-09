#ifndef ANIMATION_FADING_H_INCLUDED
#define ANIMATION_FADING_H_INCLUDED

#include <animation.h>
#include <graphics.h>
#include <TIM_AnimationDriver.h>

class CAnimationFading : public IAnimation
{
public:
    enum EFadingType{
        FADE_IN,
        FADE_OUT
    };

    TIM_AnimationDriver *aniDriver;
    const EFadingType fadingType;
    CAnimationFading(IPixelDisplay *display, TIM_AnimationDriver *aniDriver, EFadingType fadingType);
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
