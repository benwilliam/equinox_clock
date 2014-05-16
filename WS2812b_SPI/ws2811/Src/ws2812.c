

#include <stdio.h>
#include "stm32f4xx.h"
#include "ws2812.h"
//#include "main.h"
#include "stm32f4xx_tim.h"
#include "misc.h"


rgbValue_t 	ws2812ledRGB[LEDS_PHYS];			// color data for all leds

rgbValue_t 	ws2812ledOVR[LEDS_PHYS];			// overlay color data for all leds (has higher prio than ws2812ledRGB
unsigned char ws2812ledHasOVR[LEDS_PHYS];
volatile unsigned long ws2812ovrlayCounter;				// ignore overlay when 0 (decr in system ticker)

static uint16_t 		ws2812timerValues[WS2812_TIMERDMA_LEN];	// buffer for timer/dma, one byte per bit + reset pulse
volatile uint8_t		ledBusy = 0;							// = 1 while dma is sending data to leds



static void WS2812startDMA(void);									// start the dma transfer (framebuffer to timer)


//-------------------------------------------------------------------------------------------------------------

static uint16_t *rgb2pwm(uint16_t *dest, const uint8_t color)
{
	register uint8_t mask = 0x80;

	do
	{
		if (color & mask)
			*dest = WS2812_PWM_ONE;
		else
			*dest = WS2812_PWM_ZERO;

		dest++;
		mask >>= 1;
	} while (mask != 0);

	return dest;
}




// convert color codes from "ws2812_framebuffer" into serial "commands" for timer
void WS2812update(void)
{
	register uint32_t i;
	register rgbValue_t *r;
	uint16_t * bufp = ws2812timerValues;
	int c;
	uint8_t masterBrightness = 100;

	for (i = 0; i < LEDS_PHYS; i++)
	{
		if (ws2812ovrlayCounter && ws2812ledHasOVR[i])
		{
			r = (rgbValue_t *)&ws2812ledOVR[i];
			bufp = rgb2pwm(bufp, r->G);
			bufp = rgb2pwm(bufp, r->R);
			bufp = rgb2pwm(bufp, r->B);
		}
		else
		{
			r = (rgbValue_t *)&ws2812ledRGB[i];
			c = ((int)r->G * masterBrightness) / 100;
			bufp = rgb2pwm(bufp, (const uint8_t)c);
			c = ((int)r->R * masterBrightness) / 100;
			bufp = rgb2pwm(bufp, (const uint8_t)c);
			c = ((int)r->B * masterBrightness) / 100;
			bufp = rgb2pwm(bufp, (const uint8_t)c);
		}
	}

	WS2812startDMA();		// send it to RGB stripe
}





void WS2812init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef timbaseinit;
	TIM_OCInitTypeDef timocinit;
	NVIC_InitTypeDef nvic_init;

	// clear dma buffer
	int i;

	for (i = 0; i < (WS2812_TIMERDMA_LEN - WS2812_RESET_LEN); i++)
		ws2812timerValues[i] = WS2812_PWM_ZERO;
	for (; i < WS2812_TIMERDMA_LEN; i++)
		ws2812timerValues[i] = 0;

	for (; i < LEDS_PHYS; i++)
	{
		ws2812ledRGB[i].B = 0;
		ws2812ledRGB[i].G = 0;
		ws2812ledRGB[i].R = 0;
		ws2812ledOVR[i].B = 0;
		ws2812ledOVR[i].G = 0;
		ws2812ledOVR[i].R = 0;
		ws2812ledHasOVR[i] = 0;
	}

	// GPIO
	RCC_AHB1PeriphClockCmd(WS2812_RCC_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = WS2812_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(WS2812_GPIO, &GPIO_InitStructure);
	GPIO_PinAFConfig(WS2812_GPIO, GPIO_PinSource0, WS2812_GPIO_AF_TIM);

	// TIMER
	RCC_APB1PeriphClockCmd(WS2812_RCC_TIM, ENABLE);

	TIM_TimeBaseStructInit(&timbaseinit);
	timbaseinit.TIM_ClockDivision = 		TIM_CKD_DIV1;
	timbaseinit.TIM_CounterMode = 			TIM_CounterMode_Up;
	timbaseinit.TIM_Period = 				WS2812_TIM_FREQ / WS2812_OUT_FREQ;
	timbaseinit.TIM_Prescaler = 			(uint16_t)((SystemCoreClock / 2) / WS2812_TIM_FREQ) - 1; //tim3 runs at half the speed of the system ->divide by 2
	TIM_TimeBaseInit(WS2812_TIM, &timbaseinit);

	TIM_OCStructInit(&timocinit);
	timocinit.TIM_OCMode = 					TIM_OCMode_PWM1;
	timocinit.TIM_OCPolarity = 				TIM_OCPolarity_High;
	timocinit.TIM_OutputState = 			TIM_OutputState_Enable;
	timocinit.TIM_Pulse = 					0;
	WS2812_TIM_OC_INIT(WS2812_TIM, &timocinit);

	WS2812_TIM_OC_PRECFG(WS2812_TIM, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(WS2812_TIM, ENABLE);

	TIM_CCxCmd(WS2812_TIM, WS2812_TIM_CHANNEL, TIM_CCx_Enable);
	TIM_Cmd(WS2812_TIM, ENABLE);

	// DMA
	RCC_AHB1PeriphClockCmd(WS2812_RCC_DMA, ENABLE);
	TIM_DMACmd(WS2812_TIM, WS2812_DMA_SOURCE, ENABLE);
	DMA_ITConfig(WS2812_DMA_STREAM, DMA_IT_TC, ENABLE);

	// NVIC for DMA
	nvic_init.NVIC_IRQChannel = DMA1_Stream7_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_init.NVIC_IRQChannelSubPriority = 1;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);

	WS2812startDMA();
}




// transfer framebuffer data to the timer
static void WS2812startDMA(void)
{
	if (ledBusy)		// last DMA is not finished
		return;

	ledBusy = 1;
	static DMA_InitTypeDef dma_init =
	{
			.DMA_BufferSize 		= WS2812_TIMERDMA_LEN,
			.DMA_Channel 			= WS2812_DMA_CHANNEL,
			.DMA_DIR 				= DMA_DIR_MemoryToPeripheral,
			.DMA_FIFOMode 			= DMA_FIFOMode_Disable,
			.DMA_FIFOThreshold 		= DMA_FIFOThreshold_HalfFull,
			.DMA_Memory0BaseAddr 	= (uint32_t) &ws2812timerValues[0],
			.DMA_MemoryBurst 		= DMA_MemoryBurst_Single,
			.DMA_MemoryDataSize 	= DMA_MemoryDataSize_HalfWord,
			.DMA_MemoryInc 			= DMA_MemoryInc_Enable,
			.DMA_Mode 				= DMA_Mode_Normal,
			.DMA_PeripheralBaseAddr = (uint32_t) &WS2812_TIM->WS2812_CCR,
			.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single,
			.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
			.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable,
			.DMA_Priority 			= DMA_Priority_Medium
	};

	DMA_Cmd(WS2812_DMA_STREAM, DISABLE);
	while (DMA_GetCmdStatus(WS2812_DMA_STREAM) == ENABLE)
		; //wait
	DMA_Init(WS2812_DMA_STREAM, &dma_init);
	DMA_Cmd(WS2812_DMA_STREAM, ENABLE);
	TIM_DMACmd(WS2812_TIM, WS2812_DMA_SOURCE, ENABLE);
}




// gets called when dma transfer has completed
void DMA1_Stream7_IRQHandler(void)
{
	DMA_ClearITPendingBit(WS2812_DMA_STREAM, DMA_IT_TCIF7);
	DMA_Cmd(WS2812_DMA_STREAM, DISABLE);
	while (DMA_GetCmdStatus(WS2812_DMA_STREAM) == ENABLE)
		; //wait

	// need to disable this, otherwise some glitches can occur (first bit gets lost)
	TIM_DMACmd(WS2812_TIM, WS2812_DMA_SOURCE, DISABLE);

	ledBusy = 0;			// get ready for next transfer
}




void WS2812test(void)
{
	uint32_t i,j;
#define NR_TEST_PATTERNS	12
	uint8_t patterns[NR_TEST_PATTERNS][3] = {
			{0xf0,0x00,0x00},
			{0x00,0xf0,0x00},
			{0x00,0x00,0xf0},
			{0xf0,0xf0,0x00},
			{0xf0,0x00,0xf0},
			{0x00,0xf0,0xf0},
			{0xf0,0xf0,0xf0},
			{0x40,0x40,0x40},
			{0xf0,0xf0,0xf0},
			{0x40,0x40,0x40},
			{0xf0,0xf0,0xf0},
			{0x00,0x00,0x00},
	};

	for (i = 0; i < LEDS_PHYS; i++) {
		ws2812ledRGB[i].R = 0;
		ws2812ledRGB[i].G = 0;
		ws2812ledRGB[i].B = 0;
	}

	for(j=0; j<NR_TEST_PATTERNS; j++)
	{
		for (i = 0; i < LEDS_PHYS; i++)
		{
			ws2812ledRGB[i].R = patterns[j][0];
			ws2812ledRGB[i].G = patterns[j][1];
			ws2812ledRGB[i].B = patterns[j][2];
		}
		while (ledBusy)
			;
		WS2812update();
//		delay_ms(60);
	}

}
