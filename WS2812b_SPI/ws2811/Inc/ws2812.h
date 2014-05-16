

#ifndef WS2812_STM32F4_H
#define WS2812_STM32F4_H

#include "hardware.h"


// helper macro for concatenation
#define CAT_(a,b)		a ## b
#define CAT(a,b)		CAT_(a,b)


#define		LEDS_X			10
#define		LEDS_Y			1
#define		LEDS_PHYS		(2*LEDS_X+2*LEDS_Y)			// physical leds on stripe
#define		DELAY_LINE_SIZE	25

// ----------------------------- definitions -----------------------------
// warning: change source file if using tim1/8/9/10/11
// (different APB => change function calls/peripheral names/clock generation)
#define WS2812_TIMER_NR			3
#define WS2812_TIM_CHAN_NR		3
#define WS2812_OUT_PORT			B
#define WS2812_OUT_PIN			0

#define WS2812_RCC_DMA			RCC_AHB1Periph_DMA1
#define WS2812_DMA_STREAM		DMA1_Stream7
#define WS2812_DMA_CHANNEL		DMA_Channel_5

#define WS2812_TIM				CAT(TIM, 				WS2812_TIMER_NR)
#define WS2812_RCC_TIM			CAT(RCC_APB1Periph_TIM, WS2812_TIMER_NR)
#define WS2812_GPIO_AF_TIM 		CAT(GPIO_AF_TIM, 		WS2812_TIMER_NR)
#define WS2812_CCR				CAT(CCR, 				WS2812_TIM_CHAN_NR)
#define WS2812_SETCOMPARE		CAT(TIM_SetCompare, 	WS2812_TIM_CHAN_NR)
#define WS2812_DMA_SOURCE		CAT(TIM_DMA_CC, 		WS2812_TIM_CHAN_NR)
#define WS2812_TIM_CHANNEL		CAT(TIM_Channel_, 		WS2812_TIM_CHAN_NR)
#define WS2812_TIM_OC			CAT(TIM_OC, 			WS2812_TIM_CHAN_NR)
#define WS2812_TIM_OC_INIT		CAT(WS2812_TIM_OC, 		Init)
#define WS2812_TIM_OC_PRECFG	CAT(WS2812_TIM_OC, 		PreloadConfig)

#define WS2812_GPIO				CAT(GPIO, 				WS2812_OUT_PORT)
#define WS2812_RCC_GPIO			CAT(RCC_AHB1Periph_GPIO, WS2812_OUT_PORT)
#define WS2812_GPIO_PIN			CAT(GPIO_Pin_, 			WS2812_OUT_PIN)

// timer values to generate a "one" or a "zero" according to ws2812 datasheet
#define WS2812_PWM_ONE			30
#define WS2812_PWM_ZERO			15

// number of timer cycles (~1.25µs) for the reset pulse
#define WS2812_RESET_LEN		50

// three colors per led, eight bits per color
#define WS2812_TIMERDMA_LEN		(LEDS_PHYS * 3 * 8 + WS2812_RESET_LEN)

#define WS2812_TIM_FREQ			42000000
#define WS2812_OUT_FREQ			800000

typedef struct {
	uint8_t		R;
	uint8_t		G;
	uint8_t		B;
} rgbValue_t;

extern short factorI;

// ----------------------------- variables -----------------------------

extern rgbValue_t ws2812ledRGB[LEDS_PHYS];
extern rgbValue_t ws2812ledOVR[LEDS_PHYS];			// overlay color data for all leds (has higher prio than ws2812ledRGB
unsigned char ws2812ledHasOVR[LEDS_PHYS];
extern volatile uint8_t	ledBusy;					// = 1 while dma is sending data to leds
extern volatile unsigned long ws2812ovrlayCounter;	// ignore overlay when 0 (decr in system ticker)

// ----------------------------- functions -----------------------------
void WS2812init(void);
void WS2812update(void);
void WS2812test(void);

#endif
