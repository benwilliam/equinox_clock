#ifndef DISCOVERYLEDS_H
#define DISCOVERYLEDS_H

#include "stm32f4xx_gpio.h"

#define LED_GREEN      GPIO_Pin_12
#define LED_ORANGE     GPIO_Pin_13
#define LED_RED        GPIO_Pin_14
#define LED_BLUE       GPIO_Pin_15

void LEDS_init(void);
void LEDon(uint16_t LED_PIN);
void LEDoff(uint16_t LED_PIN);

#endif
