#include <animationDriver.h>
#include <algorithm>
#include <cstddef>

IAnimationDriver::IAnimationDriver(IPixelDisplay *display) : display(display),performSteps(0)
{

}

void IAnimationDriver::updateAnimations(void)
{
    while(performSteps > 0)
    {
        //C++11 method
        for (IAnimation *animation : animations)
        {
            if(animation != NULL)
            {
                animation->perofrmAnimationStep();
            }
        }

//        for(std::vector<int>::size_type i = 0; i != animations.size(); i++)
//        {
//            if(animations[i] != NULL)
//            {
//                animations[i]->perofrmAnimationStep();
//            }
//        }
        display->update();
        performSteps--;
    }


}

void IAnimationDriver::registerAnimation(IAnimation &animation)
{
    animations.push_back(&animation);
}



void IAnimationDriver::deRegisterAnimation(IAnimation &animation)
{
    std::vector<IAnimation *>::iterator position = std::find(animations.begin(), animations.end(), &animation);
    if (position != animations.end()) // == vector.end() means the element was not found
        animations.erase(position);
}


void IAnimationDriver::handleAnimationISR(void)
{
    performSteps++;
}
