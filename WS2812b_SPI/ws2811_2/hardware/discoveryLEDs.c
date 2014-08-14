#include "discoveryLEDs.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void LEDS_init(void){
	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure PD12, 13, 14 and PD15 in output pushpull mode */
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void LEDon(uint16_t LED_PIN){
	GPIOD->BSRRL = LED_PIN;
}

void LEDoff(uint16_t LED_PIN){
	GPIOD->BSRRH = LED_PIN;
}
