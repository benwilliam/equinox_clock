#ifndef TIM_AnimationDriver_H
#define TIM_AnimationDriver_H

#include "animationDriver.h"
#include <vector>

extern "C" {
    void TIM2_IRQHandler(void);
}

class TIM_AnimationDriver : public IAnimationDriver
{
public:
    /** \brief constructor
    *
    * \param display the PixelDisplay driver to show the animation on
    *
    */
    TIM_AnimationDriver(IPixelDisplay *display);

    /** \brief init RTC hardware
     *
     */
    virtual void init();


    friend void TIM2_IRQHandler(void);
private:


    static std::vector<TIM_AnimationDriver *> animationDrivers;


};
#endif //TIM_AnimationDriver_H


