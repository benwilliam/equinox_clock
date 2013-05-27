#include "stm32f4xx.h"

#define TLC5940_N 1

#if (24 * TLC5940_N > 255)
#define gsData_t uint16_t
#else
#define gsData_t uint8_t
#endif

#if (16 * TLC5940_N > 255)
#define channel_t uint16_t
#else
#define channel_t uint8_t
#endif

#if (3 * 16 * TLC5940_N > 255)
#define channel3_t uint16_t
#else
#define channel3_t uint8_t
#endif

#if (24 * TLC5940_N * TLC5940_MULTIPLEX_N > 255)
#define gsOffset_t uint16_t
#else
#define gsOffset_t uint8_t
#endif

#define gsDataSize ((gsData_t)24 * TLC5940_N)
#define numChannels ((channel_t)16 * TLC5940_N)

#if (TLC5940_INCLUDE_GAMMA_CORRECT)
extern uint16_t TLC5940_GammaCorrect[];
#endif // TLC5940_INCLUDE_GAMMA_CORRECT

#if (TLC5940_INCLUDE_DC_FUNCS)
#if (12 * TLC5940_N > 255)
#define dcData_t uint16_t
#else
#define dcData_t uint8_t
#endif

#define dcDataSize ((dcData_t)12 * TLC5940_N)

extern dcData_t dcData[dcDataSize];

void TLC5940_SetDC(channel_t channel, uint8_t value);
void TLC5940_SetAllDC(uint8_t value);
void TLC5940_ClockInDC(void);
#endif // TLC5940_INCLUDE_DC_FUNCS

//debug
void deinitMCO(void);
void initMCO(void);

void TLC5940_SendGS_DMARequest(void);
void TLC5940_UpdateGS(void);
void TLC5940_Init(void);
void TLC5940_SetGS(channel_t channel, uint16_t value);
void TLC5940_SetAllGS(uint16_t value);
