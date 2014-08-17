#include <animation.h>

IAnimation::IAnimation(IPixelDisplay *display) : display(display)
{

}


void IAnimation::perofrmAnimationStep(void)
{
    doAnimationStep();
}
