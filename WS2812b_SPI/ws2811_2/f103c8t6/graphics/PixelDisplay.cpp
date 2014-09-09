#include "PixelDisplay.h"

IPixelDisplay::IPixelDisplay(uint16_t nr_pixel) : NR_PIXEL(nr_pixel), NR_LEDS(NR_PIXEL*3)
{
    Pixel = new RGBColor[NR_PIXEL];
}

IPixelDisplay::~IPixelDisplay()
{
    delete[] Pixel;
}


void IPixelDisplay::setPixel(uint8_t pixel, const RGBColor &c)
{
    Pixel[pixel].R = c.R;
    Pixel[pixel].G = c.G;
    Pixel[pixel].B = c.B;
}

void IPixelDisplay::setAllPixel(const RGBColor &c)
{
    for(uint8_t i = 0; i<NR_PIXEL; i++)
    {
        setPixel(i, c);
    }
}

void IPixelDisplay::clearAllPixel()
{
    setAllPixel(RGBColor(0, 0, 0));
}

