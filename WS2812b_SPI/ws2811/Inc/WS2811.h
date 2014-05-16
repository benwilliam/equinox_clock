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
#define WS2811_RESET_CYCLES     8 //cycles a 1,5 µs = 12µs
#define WS2811_RESET_LEN     (WS2811_RESET_CYCLES*BIT_FACTOR)

// three colors per led, eight bits per color
#define WS2811_FRAMEBUF_LED_LEN  (NR_LEDS * BIT_FACTOR)
#define WS2811_FRAMEBUF_LEN    (WS2811_FRAMEBUF_LED_LEN + WS2811_RESET_LEN)

#define WS2811_PWM_ZERO 0x8 //0b1000
#define WS2811_PWM_ONE 0xE //0b1110


//------------------------------ structs ------------------------------
typedef struct{
	uint8_t R;
	uint8_t G;
	uint8_t B;
} color;

// ----------------------------- functions -----------------------------
void ws2811_init(void);
void updateLED(void);

// ----------------------------- graphics -----------------------------
void setLED(uint led, uint8_t r, uint8_t g, uint8_t b);
void setLED_Color(uint led, color *c);
void setLED_32(uint led, uint32_t rgb);
void setAllLED(uint8_t r, uint8_t g, uint8_t b);
void setAllLED_Color(color *c);
void setAllLED_32(uint32_t rgb);
void clearAllLED();


//debug functions
#ifdef DEBUGMODE
void startTimer(void);
uint32_t stopTimer(void);
#endif

#endif
