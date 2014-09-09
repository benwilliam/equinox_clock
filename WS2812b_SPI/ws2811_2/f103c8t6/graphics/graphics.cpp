#include "graphics.h"


//------------hilfsfunktionen------
uint8_t maxValue(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t res = 0;
    if(r>g)
        res = r;
    else
        res = g;

    if(res>b)
        return res;
    else
        return b;
}

uint8_t maxColorValue(const RGBColor &c)
{
    return maxValue(c.R, c.G, c.B);
}

uint8_t minValue(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t res = 0;
    if(r>g)
        res = g;
    else
        res = r;

    if(res>b)
        return b;
    else
        return res;
}

uint8_t minColorValue(const RGBColor &c)
{
    return minValue(c.R, c.G, c.B);
}

uint8_t sumMax(uint8_t a, uint8_t b)
{
    uint32_t res = a + b;
    if(res > MAXVALUE)
        return MAXVALUE;
    else
        return res;
}

uint8_t subMax(uint8_t a, uint8_t b)
{
    int32_t res = a - b;
    if(res < 0)
        return 0;
    else
        return res;
}

void incrementLED(RGBColor &LED, uint8_t step)
{
    LED.R = sumMax(LED.R, step);
    LED.G = sumMax(LED.G, step);
    LED.B = sumMax(LED.B, step);
}

void decrementLED(RGBColor &LED, uint8_t step)
{
    LED.R = subMax(LED.R, step);
    LED.G = subMax(LED.G, step);
    LED.B = subMax(LED.B, step);
}


//------GRAPHICS-------------

RGBColor addColor(const RGBColor &c1, const RGBColor &c2)
{
    return RGBColor(sumMax(c1.R, c2.R), sumMax(c1.G, c2.G), sumMax(c1.B, c2.B));
}


void setBrightnessofPixel(RGBColor &pixel, uint8_t brightness)
{
    pixel.R = (uint8_t) (pixel.R * brightness/100);
    pixel.G = (uint8_t) (pixel.G * brightness/100);
    pixel.B = (uint8_t) (pixel.B * brightness/100);

}

void setBrightnessOfALLPixel(RGBColor *pixels, uint16_t pixelSize, uint8_t brightness)
{
    for(uint16_t i=0; i<(pixelSize); i++)
    {
        setBrightnessofPixel(pixels[i], brightness);
    }
}


void shiftRight(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift)
{
    if(toShift == 0 || toShift>= pixelSize)
        return;

    for(int i=pixelSize-1; i>=toShift; i--)
    {
        pixels[i] = pixels[i-toShift];

    }
    for(int i=0; i<toShift; i++)
    {
        pixels[i] = BLACK;
    }
}

void shiftLeft(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift)
{
    if(toShift == 0 || toShift>= pixelSize)
        return;

    for(int i=0; i<(pixelSize-toShift); i++)
    {
        pixels[i] = pixels[i+toShift];

    }
    for(int i=(pixelSize-toShift); i<pixelSize; i++)
    {
        pixels[i] = BLACK;
    }
}

//geht mit modolu bestimmt einfacher und etwas effizienter
void shiftRightRound(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift)
{

    if(toShift == 0 || toShift>= pixelSize)
        return;

    RGBColor tempLEDs[toShift];

    for(int i=0; i<toShift; i++)
    {
        tempLEDs[i] = pixels[i];
    }
    for(int i=0; i<(pixelSize-toShift); i++)
    {
        pixels[i] = pixels[i+toShift];

    }
    for(int i=(pixelSize-toShift); i<pixelSize; i++)
    {
        pixels[i] = tempLEDs[i-(pixelSize-toShift)];
    }
}

void shiftLeftRound(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift)
{

    if(toShift == 0 || toShift>= pixelSize)
        return;

    RGBColor tempLEDs[toShift];
    for(int i=pixelSize-1; i>=(pixelSize-1)-toShift; i--)
    {
        tempLEDs[(pixelSize-1)-i] = pixels[i];
    }
    for(int i=pixelSize-1; i>=toShift; i--)
    {
        pixels[i] = pixels[i-toShift];

    }
    for(int i=0; i<toShift; i++)
    {
        pixels[i] = tempLEDs[i];
    }
}

void invertLEDColor(RGBColor &led)
{
    led.R = MAXVALUE - led.R;
    led.G = MAXVALUE - led.G;
    led.B = MAXVALUE - led.B;
}

void invertALLLEDColor(RGBColor *pixels, uint16_t pixelSize)
{
    for(int i = 0; i<pixelSize; i++)
    {
        invertLEDColor(pixels[i]);
    }
}

