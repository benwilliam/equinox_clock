#include "graphics.h"
#include "WS2811.h"
#include <RTC.h>

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

static uint8_t maxColorValue(const color *c){
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

static uint8_t minColorValue(const color *c){
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

void addColorToLED(uint8_t led, const color *c){
    LEDs[led].R = sumMax(LEDs[led].R, c->R);
    LEDs[led].G = sumMax(LEDs[led].G, c->G);
    LEDs[led].B = sumMax(LEDs[led].B, c->B);
}


void setBrightnessOfLED(uint8_t led, uint8_t brightness){
    assert_param(led < NR_LEDS);
    assert_param(brightness < =100);

    LEDs[led].R = (uint8_t) (LEDs[led].R * brightness/100);
    LEDs[led].G = (uint8_t) (LEDs[led].G * brightness/100);
    LEDs[led].B = (uint8_t) (LEDs[led].B * brightness/100);

}

void setBrightnessOfALL(uint8_t brightness){
    for(uint16_t i=0; i<(NR_PIXEL); i++)
	{
		setBrightnessOfLED(i, brightness);
	}
}

///----ANIMATION functions---///
static uint32_t currentStep = 0;
static uint32_t maxSteps = 128;

//---Fading animation---//
static int8_t stepFactor = 1;
static int8_t ledNr = 0;

void fadingStep(void){
    if(currentStep <= maxSteps){
        setBrightnessOfLED(ledNr, (currentStep*stepFactor));
        updateLED();
        currentStep++;
    }
    else{
        setAnimationCallback(0);
    }
}

//parameter "steps" is a very tricky argument. It aligns all to 128 steps, that means if steps is set e.g. to 64 that the duration of the animation is 64/128 = 0,5 second
void startFadingLED(uint8_t led, uint8_t from, uint8_t to, uint8_t steps){
    int8_t oneStep = (to-from)/steps+1;
    stepFactor = oneStep;
    currentStep = 0;
    maxSteps = steps;
    ledNr =led;
    setAnimationCallback(&fadingStep);

}



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

