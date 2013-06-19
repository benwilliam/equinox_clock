#include <stdio.h>
#include "stm32f4xx.h"
#include "WS2811.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "string.h"

#define DEBUGMODE

#ifdef DEBUGMODE
#define MILLISEKUNDE SystemCoreClock/1000
#define MICROSEKUNDE SystemCoreClock/1000000

volatile uint32_t COUNTER = 0;
void startTimer(void){
	COUNTER = 0;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(MICROSEKUNDE);
}

void SysTick_Handler(void){
	COUNTER++;
}

uint32_t stopTimer(void){
	return COUNTER;
}
#endif

typedef struct{
	uint8_t R;
	uint8_t G;
	uint8_t B;
} LED;


// local buffer for timer/dma, one byte per bit + reset pulse
static uint16_t framebuffer[WS2811_FRAMEBUF_LEN];
static LED LEDs[NR_LEDS];

// start the dma transfer (framebuffer to timer)
#define TRUE ENABLE
#define FALSE DISABLE
static FunctionalState DMA_BUSY = FALSE;


void LED_TO_PWM(void) {
	LED curLED;
	for (int i = 0; i < NR_LEDS; i++) {
		curLED = LEDs[i];

		uint8_t mask = 0x80;
		uint16_t * R;
		uint16_t * G;
		uint16_t * B;
		G = &framebuffer[i*SIZE_OF_LED];
		R = &framebuffer[i*SIZE_OF_LED+8];
		B = &framebuffer[i*SIZE_OF_LED+16];


		do {

			if (curLED.R & mask) {
				*R = WS2811_PWM_ONE;
			} else {
				*R = WS2811_PWM_ZERO;
			}

			if (curLED.G & mask) {
				*G = WS2811_PWM_ONE;
			} else {
				*G = WS2811_PWM_ZERO;
			}

			if (curLED.B & mask) {
				*B = WS2811_PWM_ONE;
			} else {
				*B = WS2811_PWM_ZERO;
			}

			R += 1;
			G += 1;
			B += 1;
			mask >>= 1;
		} while (mask != 0);
	}
}

// writes the pwm values of one byte into the array which will be used by the dma
static void color2pwm(uint16_t ** const dest, const uint8_t color) {
  uint8_t mask = 0x80;

  do {
    if (color & mask) {
      **dest = WS2811_PWM_ONE;
    } else {
      **dest = WS2811_PWM_ZERO;
    }
    *dest += 1;
    mask >>= 1;
  } while (mask != 0);
}

void setColor(uint8_t led, uint8_t r, uint8_t g, uint8_t b){
	assert_param(led <= NR_LEDS);

	uint16_t * bufp = &framebuffer[led*SIZE_OF_LED];

	color2pwm(&bufp, g);
	color2pwm(&bufp, r);
	color2pwm(&bufp, b);
}

void setColor_32(uint8_t led, uint32_t rgb){
	assert_param(rgb <= 0xffffff);

	setColor(led, (rgb & 0x00ff0000) >> 16, (rgb & 0x0000ff00) >> 8, (rgb & 0x000000ff) >> 0);
}

void setAllLEDColor(uint8_t r, uint8_t g, uint8_t b){
	for(int i = 0; i<NR_LEDS; i++)
	{
		setColor(i, r, g, b);
	}
}

void setAllLEDColor_32(uint32_t rgb){
	for(int i = 0; i<NR_LEDS; i++)
	{
		setColor_32(i, rgb);
	}
}


void clearColor(){
	for (int i = 0; i < (WS2811_FRAMEBUF_LED_LEN); i++)
	    framebuffer[i] = WS2811_PWM_ZERO;

}


// initialization of peripherals for ws2811:
// GPIO, timer (PWM mode), DMA, NVIC
void ws2811_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef timbaseinit;
  TIM_OCInitTypeDef timocinit;
  NVIC_InitTypeDef nvic_init;

  // data init, make sure reset pulse is zero
  int i;
  for (i = 0; i < (WS2811_FRAMEBUF_LED_LEN); i++)
    framebuffer[i] = WS2811_PWM_ZERO;
  for (; i < WS2811_FRAMEBUF_LEN; i++)
    framebuffer[i] = 0;

  // GPIO
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

  // TIMER
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseStructInit(&timbaseinit);
  timbaseinit.TIM_ClockDivision = TIM_CKD_DIV1;
  timbaseinit.TIM_CounterMode = TIM_CounterMode_Up;
  timbaseinit.TIM_Period = WS2811_PWM_PERIOD;//(WS2811_TIM_FREQ / WS2811_OUT_FREQ)-1;
  timbaseinit.TIM_Prescaler = (uint16_t)(
      (SystemCoreClock / 8) / WS2811_TIM_FREQ) - 1; //sysclock / 4 (APB1 divider) / 2 (timer divider)
  TIM_TimeBaseInit(TIM3, &timbaseinit);

  TIM_OCStructInit(&timocinit);
  timocinit.TIM_OCMode = TIM_OCMode_PWM1;
  timocinit.TIM_OCPolarity = TIM_OCPolarity_High;
  timocinit.TIM_OutputState = TIM_OutputState_Enable;
  timocinit.TIM_Pulse = 0;
  TIM_OC3Init(TIM3, &timocinit);

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);
  TIM_Cmd(TIM3, ENABLE);

  // DMA
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  TIM_DMACmd(TIM3, TIM_DMA_CC3, ENABLE);
  DMA_ITConfig(DMA1_Stream7, DMA_IT_TC, ENABLE);

  // NVIC for DMA
  nvic_init.NVIC_IRQChannel = DMA1_Stream7_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 4;
  nvic_init.NVIC_IRQChannelSubPriority = 0;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
}


// begin transferring framebuffer data to the timer
void start_dma(void)
{
	if(!DMA_BUSY){
		DMA_BUSY = TRUE;
	  static DMA_InitTypeDef dma_init =
	  {
		  .DMA_BufferSize     = WS2811_FRAMEBUF_LEN,
		  .DMA_Channel       = DMA_Channel_5,
		  .DMA_DIR         = DMA_DIR_MemoryToPeripheral,
		  .DMA_FIFOMode       = DMA_FIFOMode_Disable,
		  .DMA_FIFOThreshold     = DMA_FIFOThreshold_HalfFull,
		  .DMA_Memory0BaseAddr   = (uint32_t) framebuffer,
		  .DMA_MemoryBurst     = DMA_MemoryBurst_Single,
		  .DMA_MemoryDataSize   = DMA_MemoryDataSize_HalfWord,
		  .DMA_MemoryInc       = DMA_MemoryInc_Enable,
		  .DMA_Mode         = DMA_Mode_Normal,
		  .DMA_PeripheralBaseAddr = (uint32_t) &TIM3->CCR3,
		  .DMA_PeripheralBurst   = DMA_PeripheralBurst_Single,
		  .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
		  .DMA_PeripheralInc     = DMA_PeripheralInc_Disable,
		  .DMA_Priority       = DMA_Priority_Medium
	  };

	  DMA_Cmd(DMA1_Stream7, DISABLE);
	  while (DMA_GetCmdStatus(DMA1_Stream7) == ENABLE)
		; //wait
	  DMA_Init(DMA1_Stream7, &dma_init);
	  DMA_Cmd(DMA1_Stream7, ENABLE);
	  TIM_DMACmd(TIM3, TIM_DMA_CC3, ENABLE);
	}
}

// gets called when dma transfer has completed
void DMA1_Stream7_IRQHandler(void)
{
  DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF7);
  DMA_Cmd(DMA1_Stream7, DISABLE);

  // need to disable this, otherwise some glitches can occur (first bit gets lost)
  TIM_DMACmd(TIM3, TIM_DMA_CC3, DISABLE);

  DMA_BUSY = FALSE;
}
