#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <PixelDisplay.h>

#define WHITE	RGBColor(255,255,255)
#define YELLOW	RGBColor(255,255,0)
#define ORANGE	RGBColor(255,165,0)
#define FUCHSIA	RGBColor(255,0,255)
#define RED		RGBColor(255,0,0)
#define SILVER	RGBColor(192,192,192)
#define GRAY	RGBColor(128,128,128)
#define OLIVE	RGBColor(128,128,0)
#define PURPLE	RGBColor(128,0,128)
#define MAROON	RGBColor(128,0,0)
#define AQUA	RGBColor(0,255,255)
#define LIME	RGBColor(50,205,50)
#define TEAL	RGBColor(0,128,128)
#define GREEN	RGBColor(0,255,0)
#define BLUE	RGBColor(0,0,255)
#define NAVY	RGBColor(0,0,128)
#define BLACK	RGBColor(0,0,0)

#define DEFAULT_COLOR BLACK

#define MAXVALUE 255
#define MINVALUE 0

void incrementLED(RGBColor &LED, uint8_t step);
void decrementLED(RGBColor &LED, uint8_t step);

RGBColor addColor(const RGBColor &c1, const RGBColor &c2);
void setBrightnessofPixel(RGBColor &pixel, uint8_t brightness);
void setBrightnessOfALLPixel(RGBColor *pixels, uint16_t pixelSize, uint8_t brightness);

void shiftRight(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift);
void shiftLeft(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift);
void shiftRightRound(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift);
void shiftLeftRound(RGBColor *pixels, uint16_t pixelSize, uint8_t toShift);
void invertLEDColor(RGBColor &led);
void invertALLLEDColor(RGBColor *pixels, uint16_t pixelSize);

#endif
