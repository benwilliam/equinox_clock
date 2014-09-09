
#include "animation_knightRider.h"

CAnimationKnightRider::CAnimationKnightRider(IPixelDisplay *display, RGBColor color) :  IAnimation(display),color(color)
{


}

void CAnimationKnightRider::doAnimationStep(void)
{
    if((currentstep % 5)==0)
    {

        if(currentPixel == 0 && direction == 1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel, color);
        }
        else if(currentPixel == 1 && direction == 1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel, color);
            display->setPixel(currentPixel-1, color);
            setBrightnessofPixel(display->Pixel[currentPixel-1], 50);
        }
        else if(currentPixel == 2 && direction == 1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel, color);
            display->setPixel(currentPixel-1, color);
            setBrightnessofPixel(display->Pixel[currentPixel-1], 50);
            display->setPixel(currentPixel-2, color);
            setBrightnessofPixel(display->Pixel[currentPixel-2], 25);
        }
        else if(currentPixel > 2 && currentPixel < display->NR_PIXEL-1 && direction == 1)
        {
            shiftRight(display->Pixel, display->NR_PIXEL, 1);
        }
        else if(currentPixel == display->NR_PIXEL-1 && direction == 1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel, color);
            display->setPixel(currentPixel-1, color);
            setBrightnessofPixel(display->Pixel[currentPixel-1], 50);
        }
        else if(currentPixel == display->NR_PIXEL && direction == 1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel-1, color);
            direction = -1;
            currentPixel = currentPixel + direction;
        }
        else if(currentPixel == display->NR_PIXEL-2 && direction == -1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel, color);
            display->setPixel(currentPixel+1, color);
            setBrightnessofPixel(display->Pixel[currentPixel+1], 50);
        }
        else if(currentPixel == display->NR_PIXEL-3 && direction == -1)
        {
            display->clearAllPixel();
            display->setPixel(currentPixel, color);
            display->setPixel(currentPixel+1, color);
            setBrightnessofPixel(display->Pixel[currentPixel+1], 50);
            display->setPixel(currentPixel+2, color);
            setBrightnessofPixel(display->Pixel[currentPixel+2], 25);
        }
        else if(currentPixel >=0 &&  currentPixel < display->NR_PIXEL-3 && direction == -1)
        {
            shiftLeft(display->Pixel, display->NR_PIXEL, 1);
        }
        else if(currentPixel == -1 && direction == -1)
        {
            display->clearAllPixel();
            display->setPixel(0, color);
            display->setPixel(1, color);
            setBrightnessofPixel(display->Pixel[1], 50);
        }
        else if(currentPixel == -2 && direction == -1)
        {
            display->clearAllPixel();
            display->setPixel(0, color);
            direction =1;
            currentPixel = currentPixel + direction;
        }

        currentPixel = currentPixel + direction;
    }
    currentstep++;

}
