#ifndef ANIMATION_KNIGHTRIDER_H_INCLUDED
#define ANIMATION_KNIGHTRIDER_H_INCLUDED

#include <animation.h>
#include <graphics.h>
#include <TIM_AnimationDriver.h>

class CAnimationKnightRider : public IAnimation
{
public:

    CAnimationKnightRider(IPixelDisplay *display, RGBColor color);


protected:
    virtual void doAnimationStep(void);

private:
    uint16_t currentstep = 0;
    const RGBColor color;
    int64_t currentPixel = 0;
    int8_t direction = 1;
};




#endif /* ANIMATION_KNIGHTRIDER_H_INCLUDED */
