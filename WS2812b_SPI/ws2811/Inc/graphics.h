#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "WS2811.h"

#define WHITE	(color){255,255,255}
#define YELLOW	(color){255,255,0}
#define ORANGE	(color){255,165,0}
#define FUCHSIA	(color){255,0,255}
#define RED		(color){255,0,0}
#define SILVER	(color){192,192,192}
#define GRAY	(color){128,128,128}
#define OLIVE	(color){128,128,0}
#define PURPLE	(color){128,0,128}
#define MAROON	(color){128,0,0}
#define AQUA	(color){0,255,255}
#define LIME	(color){0,255,0}
#define TEAL	(color){0,128,128}
#define GREEN	(color){0,255,0}
#define BLUE	(color){0,0,255}
#define NAVY	(color){0,0,128}
#define BLACK	(color){0,0,0}

#define MAXVALUE 255
#define MINVALUE 0


void shiftRight(uint8_t toShift);
void shiftLeft(uint8_t toShift);

#endif
