#ifndef WS2811_STM32F4_H
#define WS2811_STM32F4_H

#include "stdint.h"
// ----------------------------- definitions -----------------------------
#define NR_LEDS      60
#define SIZE_OF_LED 24 // 3(RGB) * 8 Bit

// timer values to generate a "one" or a "zero" according to ws2812 datasheet
#define WS2811_PWM_PERIOD	19
#define WS2811_PWM_ZERO     (WS2811_PWM_PERIOD / 5 + 1)  // 0.25 탎 of 1.25탎 is high => 1/5 of the period
#define WS2811_PWM_ONE      (WS2811_PWM_PERIOD - WS2811_PWM_ZERO) // 1탎 of 1.25탎 is high -> 4/5 of the period


// number of timer cycles (~1.25탎) for the reset pulse
#define WS2811_RESET_LEN    1 //delay comes from LED_TO_PWM() function ca. 106 탎

// three colors per led, eight bits per color
#define WS2811_FRAMEBUF_LED_LEN  (NR_LEDS * SIZE_OF_LED)
#define WS2811_FRAMEBUF_LEN    (WS2811_FRAMEBUF_LED_LEN + WS2811_RESET_LEN)


#define WS2811_TIM_FREQ      21000000
#define WS2811_OUT_FREQ        800000

#define WS2811_DATATRANSFER_DURATION 2000 //in 킪
#define WS2811_FRAMERATE	(1000000 / WS2811_DATATRANSFER_DURATION) //in Frames/second


// ----------------------------- functions -----------------------------
void ws2811_init(void);
void setColor(uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void setColor_32(uint8_t led, uint32_t rgb);
void setAllLEDColor(uint8_t r, uint8_t g, uint8_t b);
void setAllLEDColor_32(uint32_t rgb);
void clearColor();

void start_dma();
void LED_TO_PWM(void);
void startTimer(void);
uint32_t stopTimer(void);
#endif
