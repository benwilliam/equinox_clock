#include "WS2812Interface.h"


IWS2812Interface::IWS2812Interface(uint16_t nr_pixel) : NR_PIXEL(nr_pixel){
    Pixel = new RGBColor[NR_PIXEL];
}

IWS2812Interface::~IWS2812Interface(){
    delete[] Pixel;
}


void IWS2812Interface::setPixel(uint8_t pixel, const RGBColor &c){
	Pixel[pixel].R = c.R;
	Pixel[pixel].G = c.G;
	Pixel[pixel].B = c.B;
}

void IWS2812Interface::setAllPixel(const RGBColor &c){
	for(uint8_t i = 0; i<NR_PIXEL; i++)
	{
		setPixel(i, c);
	}
}

void IWS2812Interface::clearAllPixel(){
	setAllPixel(RGBColor(0, 0, 0));
}

