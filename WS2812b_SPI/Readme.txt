STM32F407 Discovery Board + WS2812b LED Stripe

controlling the LEDs with SPI3 + DMA1_Stream5

CPUClock: 168 MHz
SPI_divider: 16 = 2,625 MHz

4 Bit are used to create one WS2812b Bit

[SPI] -> [WS2812b]
0001  -> ZERO
0111  -> ONE

that results in a period time of ~ 1,53 µs (instead of 1,25 µs)

delay latch time is minimum 8 periods = 8*1,53= 12,2 µs (instead of 50 µs) 
since other calculations taking so long, there is no special handling for latches

with 60 RGB LEDS that means a framerate of: ~3477 fps

all timing details for WS2812b are here:
http://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/


ws2812b_2 is a complet redisign and a port to C++, with an animation engine
