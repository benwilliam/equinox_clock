#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "WS2811.h"

#define WHITE	createColor(255,255,255)
#define YELLOW	createColor(255,255,0)
#define ORANGE	createColor(255,165,0)
#define FUCHSIA	createColor(255,0,255)
#define RED		createColor(255,0,0)
#define SILVER	createColor(192,192,192)
#define GRAY	createColor(128,128,128)
#define OLIVE	createColor(128,128,0)
#define PURPLE	createColor(128,0,128)
#define MAROON	createColor(128,0,0)
#define AQUA	createColor(0,255,255)
#define LIME	createColor(50,205,50)
#define TEAL	createColor(0,128,128)
#define GREEN	createColor(0,255,0)
#define BLUE	createColor(0,0,255)
#define NAVY	createColor(0,0,128)
#define BLACK	createColor(0,0,0)

#define DEFAULT_COLORBLACK

#define MAXVALUE 255
#define MINVALUE 0

void addColorToLED(uint8_t led, const color *c);
void setBrightnessOfLED(uint8_t led, uint8_t brightness);
void setBrightnessOfALL(uint8_t brightness);

void shiftRight(uint8_t toShift);
void shiftLeft(uint8_t toShift);

#endif
