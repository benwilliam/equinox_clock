#include "graphics.h"

extern color LEDs[];

//------------hilfsfunktionen------
static uint8_t maxValue(uint8_t r, uint8_t g, uint8_t b)
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

static uint8_t maxColorValue(color *c){
	return maxValue(c->R, c->G, c->B);
}

static uint8_t minValue(uint8_t r, uint8_t g, uint8_t b)
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

static uint8_t minColorValue(color *c){
	return minValue(c->R, c->G, c->B);
}

static uint8_t sumMax(uint8_t a, uint8_t b)
{
	uint32_t res = a + b;
	if(res > MAXVALUE)
		return MAXVALUE;
	else
		return res;
}

static uint8_t subMax(uint8_t a, uint8_t b)
{
	int32_t res = a - b;
	if(res < 0)
		return 0;
	else
		return res;
}

static void incrementLED(color *LED, uint8_t step)
{
	LED->R = sumMax(LED->R, step);
	LED->G = sumMax(LED->G, step);
	LED->B = sumMax(LED->B, step);
}

static void decrementLED(color *LED, uint8_t step)
{
	LED->R = subMax(LED->R, step);
	LED->G = subMax(LED->G, step);
	LED->B = subMax(LED->B, step);
}


//------GRAPHICS-------------

void shiftRight(uint8_t toShift){
	assert_param(toShift < NR_LEDS);
	assert_param(toShift != 0);

	if(toShift == 0 || toShift>= NR_LEDS)
		return;

	for(int i=NR_LEDS-1; i>=toShift; i--)
	{
		LEDs[i] = LEDs[i-toShift];

	}
	for(int i=0; i<toShift; i++)
	{
		LEDs[i] = BLACK;
	}
}

void shiftLeft(uint8_t toShift){
	assert_param(toShift < NR_LEDS);
	assert_param(toShift != 0);

	if(toShift == 0 || toShift>= NR_LEDS)
		return;

	for(int i=0; i<(NR_LEDS-toShift); i++)
	{
		LEDs[i] = LEDs[i+toShift];

	}
	for(int i=(NR_LEDS-toShift); i<NR_LEDS; i++)
	{
		LEDs[i] = BLACK;
	}
}

//geht mit modolu bestimmt einfacher und etwas effizienter
void shiftRightRound(uint8_t toShift){
	assert_param(toShift < NR_LEDS);
	assert_param(toShift != 0);

	if(toShift == 0 || toShift>= NR_LEDS)
		return;

	color tempLEDs[toShift];

	for(int i=0; i<toShift; i++)
	{
		tempLEDs[i] = LEDs[i];
	}
	for(int i=0; i<(NR_LEDS-toShift); i++)
	{
		LEDs[i] = LEDs[i+toShift];

	}
	for(int i=(NR_LEDS-toShift); i<NR_LEDS; i++)
	{
		LEDs[i] = tempLEDs[i-(NR_LEDS-toShift)];
	}
}

void shiftLeftRound(uint8_t toShift){
	assert_param(toShift < NR_LEDS);
	assert_param(toShift != 0);

	if(toShift == 0 || toShift>= NR_LEDS)
		return;

	color tempLEDs[toShift];
	for(int i=NR_LEDS-1; i>=(NR_LEDS-1)-toShift; i--)
	{
		tempLEDs[(NR_LEDS-1)-i] = LEDs[i];
	}
	for(int i=NR_LEDS-1; i>=toShift; i--)
	{
		LEDs[i] = LEDs[i-toShift];

	}
	for(int i=0; i<toShift; i++)
	{
		LEDs[i] = tempLEDs[i];
	}
}

void invertLEDColor(uint8_t led){
	assert_param(led < NR_LEDS);

	LEDs[led].R = MAXVALUE - LEDs[led].R;
	LEDs[led].G = MAXVALUE - LEDs[led].G;
	LEDs[led].B = MAXVALUE - LEDs[led].B;
}

void invertALLLEDColor(void){
	for(int i = 0; i<NR_LEDS; i++)
	{
		invertLEDColor(i);
	}
}

//durationFactor will be multiplicate with fadingsteps
void fadeLiniearDown(uint8_t led, uint8_t durationFactor, color *c){
	assert_param(led < NR_LEDS);

	//preparation
//	if(duration < WS2811_DATATRANSFER_DURATION*MAXVALUE/1000)
//		duration =  WS2811_DATATRANSFER_DURATION*MAXVALUE/1000;

	//how many fadingsteps are needed?
	uint8_t fadingSteps = minColorValue(c);



	//ToDo: start fading :)

}
