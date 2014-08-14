#ifndef WS2811_H
#define WS2811_H

#include "stdint.h"
#include <stdio.h>

//Attention!! Hardware dependancy!!
#include "stm32f4xx.h"

// ----------------------------- definitions -----------------------------
#define NR_PIXEL 10
#define COLORS      3 //count of colors here RGB = 3
#define NR_LEDS      (NR_PIXEL*COLORS)
#define BIT_FACTOR 4 //how many bits are used to represent one WS2812B bit
#define SIZE_OF_LED (COLORS*BIT_FACTOR) // 3(RGB) * 4 Bit(4 SPI bit per WS2812B bit)

// time for reset delay latch
#define WS2811_RESET_LEN     1 //send one extra byte which is set to 0 to hold MOSI pin to low and trigger the WS2811 latch

// three colors per led, eight bits per color
#define WS2811_FRAMEBUF_LED_LEN  (NR_LEDS * BIT_FACTOR)
#define WS2811_FRAMEBUF_LEN    (WS2811_FRAMEBUF_LED_LEN + WS2811_RESET_LEN)

#define WS2811_PWM_ZERO 0x1 //0b0001
#define WS2811_PWM_ONE 0x7 //0b0111


//------------------------------ structs ------------------------------
typedef struct{
	uint8_t G;
	uint8_t R;
	uint8_t B;
} color;

// ----------------------------- functions -----------------------------
void ws2811_init(void);
void updateLED(void);

// ----------------------------- graphics -----------------------------
void setLED(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void setLED_Color(uint8_t led, color *c);
void setLED_32(uint8_t led, uint32_t rgb);
void setAllLED(uint8_t r, uint8_t g, uint8_t b);
void setAllLED_Color(color *c);
void setAllLED_32(uint32_t rgb);
void clearAllLED();
color createColor(uint8_t r, uint8_t g, uint8_t b);
extern color LEDs[];

//debug functions
#ifdef DEBUGMODE
void startTimer(void);
uint32_t stopTimer(void);
#endif

#endif
