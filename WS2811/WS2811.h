#ifndef WS2811_H
#define WS2811_H

#include "stdint.h"

//Attention!! Hardware dependancy!!
#include "stm32f4xx.h"

// ----------------------------- definitions -----------------------------
#define NR_LEDS      60
#define SIZE_OF_LED 24 // 3(RGB) * 8 Bit

// timer values to generate a "one" or a "zero" according to ws2812 datasheet
#define WS2811_PWM_PERIOD	38 //timer period time, don't know why 38 but it works with 168 MHz Systemclock
#define WS2811_PWM_ZERO     (WS2811_PWM_PERIOD / 5 + 1)  // 0.5 탎 of 2.5탎 is high => 1/5 of the period
#define WS2811_PWM_ONE      (WS2811_PWM_PERIOD - WS2811_PWM_ZERO) // 2탎 of 2.5탎 is high -> 4/5 of the period


// number of timer cycles (~2.5탎) for the reset pulse
#define WS2811_RESET_LEN    1 //delay comes from LED_TO_PWM() function ca. 106 탎

// three colors per led, eight bits per color
#define WS2811_FRAMEBUF_LED_LEN  (NR_LEDS * SIZE_OF_LED)
#define WS2811_FRAMEBUF_LEN    (WS2811_FRAMEBUF_LED_LEN + WS2811_RESET_LEN)


#define WS2811_TIM_FREQ      21000000
#define WS2811_OUT_FREQ        400000

#define WS2811_BIT_TRANSFER_DURATION 2500 //in nS per Bit
#define WS2811_DATA_TRANSFER_DURATION (WS2811_BIT_TRANSFER_DURATION*WS2811_FRAMEBUF_LEN/1000) //complete Frame Transfer duration in 킪
#define WS2811_FRAMERATE	(1000000 / WS2811_DATATRANSFER_DURATION) //in Frames/second

//------------------------------ structs ------------------------------
typedef struct{
	uint8_t R;
	uint8_t G;
	uint8_t B;
} color;

// ----------------------------- functions -----------------------------
void ws2811_init(void);
void LED_TO_PWM(void);

// ----------------------------- graphics -----------------------------
void setLED(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void setLED(uint8_t led, color c);
void setLED_32(uint8_t led, uint32_t rgb);
void setAllLED(uint8_t r, uint8_t g, uint8_t b);
void setAllLED(color c);
void setAllLED_32(uint32_t rgb);
void clearAllLED();


//debug functions
#ifdef DEBUGMODE
void startTimer(void);
uint32_t stopTimer(void);
#endif

#endif
