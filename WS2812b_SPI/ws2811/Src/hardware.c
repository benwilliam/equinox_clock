/*****************************************************
 *
 *	Control program for the PitSchuLight TV-Backlight
 *	(c) Peter Schulten, Mülheim, Germany
 *	peter_(at)_pitschu.de
 *
 *	Die unveränderte Wiedergabe und Verteilung dieses gesamten Sourcecodes
 *	in beliebiger Form ist gestattet, sofern obiger Hinweis erhalten bleibt.
 *
 * 	Ich stelle diesen Sourcecode kostenlos zur Verfügung und biete daher weder
 *	Support an noch garantiere ich für seine Funktionsfähigkeit. Außerdem
 *	übernehme ich keine Haftung für die Folgen seiner Nutzung.

 *	Der Sourcecode darf nur zu privaten Zwecken verwendet und modifiziert werden.
 *	Darüber hinaus gehende Verwendung bedarf meiner Zustimmung.
 *
 *	History
 *	09.06.2013	pitschu		Start of work
 */

#include "hardware.h"

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED_GRN_GPIO_PORT, LED_ORN_GPIO_PORT, LED_RED_GPIO_PORT, LED_BLU_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED_GRN_PIN, LED_ORN_PIN, LED_RED_PIN, LED_BLU_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED_GRN_GPIO_CLK, LED_ORN_GPIO_CLK, LED_RED_GPIO_CLK, LED_BLU_GPIO_CLK};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT };

const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN };

const uint32_t BUTTON_CLK[BUTTONn] = {USER_BUTTON_GPIO_CLK };


//const uint8_t BUTTON_PORT_SOURCE[BUTTONn] = {USER_BUTTON_EXTI_PORT_SOURCE};

//const uint8_t BUTTON_PIN_SOURCE[BUTTONn] = {USER_BUTTON_EXTI_PIN_SOURCE };
//const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn };




void STM_EVAL_LEDInit(Led_TypeDef Led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(GPIO_CLK[Led], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}



void STM_EVAL_LEDOn(Led_TypeDef Led)
{
	GPIO_PORT[Led]->BSRRL = GPIO_PIN[Led];
}



void STM_EVAL_LEDOff(Led_TypeDef Led)
{
	GPIO_PORT[Led]->BSRRH = GPIO_PIN[Led];
}



//void STM_EVAL_LEDToggle(Led_TypeDef Led)
//{
//	GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
//}
//
//
//
//void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	/* Enable the BUTTON Clock */
//	RCC_AHB1PeriphClockCmd(BUTTON_CLK[Button], ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//
//	/* Configure Button pin as input */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
//	GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);
//
//	if (Button_Mode == BUTTON_MODE_EXTI)
//	{
//		/* Connect Button EXTI Line to Button GPIO Pin */
//		SYSCFG_EXTILineConfig(BUTTON_PORT_SOURCE[Button], BUTTON_PIN_SOURCE[Button]);
//
//		/* Configure Button EXTI line */
//		EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//		EXTI_Init(&EXTI_InitStructure);
//
//		/* Enable and set Button EXTI Interrupt to the lowest priority */
//		NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//
//		NVIC_Init(&NVIC_InitStructure);
//	}
//}



//uint32_t STM_EVAL_PBGetState(Button_TypeDef Button)
//{
//	return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
//}



//void BoardHardwareInit(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	// debug and time measurement
//	RCC_AHB1PeriphClockCmd(DEBUG_PIN_GPIO_CLK, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = DEBUG_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(DEBUG_PIN_GPIO_PORT, &GPIO_InitStructure);
//	DEBUG_OFF;
//
//	RCC_AHB1PeriphClockCmd(AUDIO_RESET_GPIO_CLK, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = AUDIO_RESET_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(AUDIO_RESET_GPIO, &GPIO_InitStructure);
//	GPIO_WriteBit(AUDIO_RESET_GPIO, AUDIO_RESET_PIN, Bit_RESET);	// set Audio reset pin low
//
//}


