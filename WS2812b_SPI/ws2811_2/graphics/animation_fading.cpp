#include "animation_fading.h"
#include "graphics.h"

CAnimationFading::CAnimationFading(IPixelDisplay *display, RTC_AnimationDriver *rtc, EFadingType fadingType)
    : IAnimation(display), rtc(rtc), fadingType(fadingType)
{

}
void CAnimationFading::doAnimationStep(void)
{

 switch(fadingType)
    {
    case FADE_IN:
        fadeIn();
        break;
    case FADE_OUT:
        fadeOut();
        break;
    default:
        fadeIn();;
        break;
    }



}

void CAnimationFading::setuptading(uint16_t pixel, uint16_t steps)
{
    this->pixel = pixel;
    this->steps = steps;
    switch(fadingType)
    {
    case FADE_IN:
        currentstep = 0;
        break;
    case FADE_OUT:
        currentstep = steps;
        break;
    default:
        currentstep = 0;
        break;
    }
}

void CAnimationFading::fadeIn()
{
    if(currentstep < steps)
    {
        setBrightnessofPixel(display->Pixel[pixel],(100/steps)*currentstep);
        currentstep++;
    }
}
void CAnimationFading::fadeOut()
{
    if(currentstep > 0)
    {
        setBrightnessofPixel(display->Pixel[pixel],(100/steps)*currentstep);
        currentstep--;
    }
}
