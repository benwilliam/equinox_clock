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
*	12.06.2013	pitschu		Start of work
*/


#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma.h"
//#include "i2c1.h"
#include "stdio.h"

#define CORE_CycleCounEn()    ((*((u32*)0xE0001000)) |= 1)
#define CORE_CycleCounDis()   ((*((u32*)0xE0001000)) &= ~1)
#define CORE_GetCycleCount()   (*((u32*)0xE0001004))

/*
TVP5150 TV processor:
	  PA4  -> DCMI_HSYNC  = TVP5150 HSYNC (HREF)
	  PA6  -> DCMI_PCLK   = TVP5150 PCLK (PIXCLK)
	  PB6  -> DCMI_D5     = TVP5150 D5
	  PB7  -> DCMI_VSYNC  = TVP5150 VSYNC
	  PB8  -> I2C1-SCL    = TVP5150 I2C-SCL
	  PB9  -> I2C1-SDA    = TVP5150 I2C-SDA
	  PC6  -> DCMI_D0     = TVP5150 D0
	  PC7  -> DCMI_D1     = TVP5150 D1
	  PC8  -> DCMI_D2     = TVP5150 D2
	  PC9  -> DCMI_D3     = TVP5150 D3
	  PE4  -> DCMI_D4     = TVP5150 D4
	  PE5  -> DCMI_D6     = TVP5150 D6
	  PE6  -> DCMI_D7     = TVP5150 D7 (!! I2C select during reset: 0 = 0xB8, 1 = 0xBA)

	  PE12 -> gpio (in)	  = TVP5150 AVID
	  PE13 -> gpio (in)	  = TVP5150 VBLK
	  PE14 -> gpio (out)  = TVP5150 Reset
	  PE15 -> gpio (in)	  = TVP5150 FID

	  DMA : entweder DMA2_STREAM1_CHANNEL1
			oder     DMA2_STREAM7_CHANNEL1

RGB-LED:
	  uses TIM3 Channel 3
	  PB0  -> TM3_CH3	  = PWM output to WS2812 led strip

	  DMA : uses DMA1_Strem7_Channel5

IR-Decoder
	  PE9  -> TM1_CH1	  = Input capture compare

On board leds:
	  PD12		green
	  PD13		orange
	  PD14		red
	  PD15		blue

UART3
	  PD8 -> 	TX
	  PD9 -> 	RX

--------------------------------------------------------------*/


/***** Basic hardware on discovery board */

typedef enum
{
  LED_GRN = 0,
  LED_ORN = 1,
  LED_RED = 2,
  LED_BLU = 3
} Led_TypeDef;

typedef enum
{
  BUTTON_USER = 0,
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;


#define LEDn                             4

#define LED_GRN_PIN                         GPIO_Pin_12
#define LED_GRN_GPIO_PORT                   GPIOD
#define LED_GRN_GPIO_CLK                    RCC_AHB1Periph_GPIOD

#define LED_ORN_PIN                         GPIO_Pin_13
#define LED_ORN_GPIO_PORT                   GPIOD
#define LED_ORN_GPIO_CLK                    RCC_AHB1Periph_GPIOD

#define LED_RED_PIN                         GPIO_Pin_14
#define LED_RED_GPIO_PORT                   GPIOD
#define LED_RED_GPIO_CLK                    RCC_AHB1Periph_GPIOD

#define LED_BLU_PIN                         GPIO_Pin_15
#define LED_BLU_GPIO_PORT                   GPIOD
#define LED_BLU_GPIO_CLK                    RCC_AHB1Periph_GPIOD

#define BUTTONn                         1
#define USER_BUTTON_PIN                	GPIO_Pin_0
#define USER_BUTTON_GPIO_PORT          	GPIOA
#define USER_BUTTON_GPIO_CLK           	RCC_AHB1Periph_GPIOA
#define USER_BUTTON_EXTI_LINE          	EXTI_Line0
#define USER_BUTTON_EXTI_PORT_SOURCE   	EXTI_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE    	EXTI_PinSource0
#define USER_BUTTON_EXTI_IRQn          	EXTI0_IRQn

#define DEBUG_PIN                		GPIO_Pin_1
#define DEBUG_PIN_GPIO_PORT          	GPIOB
#define DEBUG_PIN_GPIO_CLK           	RCC_AHB1Periph_GPIOB
#define	DEBUG_ON				  		DEBUG_PIN_GPIO_PORT->BSRRH = DEBUG_PIN
#define	DEBUG_OFF				  		DEBUG_PIN_GPIO_PORT->BSRRL = DEBUG_PIN
#define	DEBUG_TOGGLE			  		DEBUG_PIN_GPIO_PORT->ODR ^= DEBUG_PIN

/* Audio Reset Pin definition: pin must be held low because some pins are used by DCMI */
#define AUDIO_RESET_GPIO_CLK           RCC_AHB1Periph_GPIOD
#define AUDIO_RESET_PIN                GPIO_Pin_4
#define AUDIO_RESET_GPIO               GPIOD



void BoardHardwareInit(void);




//--------------------------------------------------------------
#define TVP5150_DCMI_BASE_ADR        ((uint32_t)DCMI_BASE)
#define TVP5150_DCMI_REG_DR_OFFSET   0x28  // DR-Register for DMA
#define TVP5150_DCMI_REG_DR_ADDRESS  (TVP5150_DCMI_BASE_ADR + TVP5150_DCMI_REG_DR_OFFSET)




// externs in delay.c
extern volatile uint32_t system_time;
void init_systick(void);
void DelayCountInit(void);
void delay_us(uint32_t time_us);
void delay_ms(uint16_t time_ms);



#endif /* HARDWARE_H_ */
