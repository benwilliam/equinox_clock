#include "WS2811.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"
#include "misc.h"

//#define DEBUGMODE

#ifdef DEBUGMODE
#define MILLISEKUNDE SystemCoreClock/1000
#define MICROSEKUNDE SystemCoreClock/1000000

volatile uint32_t COUNTER = 0;
void startTimer(void){
	COUNTER = 0;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(MICROSEKUNDE);
}

void SysTick_Handler(void){
	COUNTER++;
}

uint32_t stopTimer(void){
	return COUNTER;
}
#endif

// local buffer for timer/dma, one byte per bit + reset pulse
static uint16_t framebuffer[WS2811_FRAMEBUF_LEN];
static color LEDs[NR_PIXEL];

//just other pointer type
static uint8_t pLEDs[NR_LEDS];
pLEDs = (uint8_t *) LEDs;

// start the dma transfer (framebuffer to timer)
#define TRUE ENABLE
#define FALSE DISABLE
static FunctionalState DMA_BUSY = FALSE;

void setLED(uint led, uint8_t r, uint8_t g, uint8_t b){
	assert_param(led <= NR_PIXEL);

	LEDs[led].R = r;
	LEDs[led].G = g;
	LEDs[led].B = b;
}

void setLED_Color(uint led, color *c){
	assert_param(led <= NR_PIXEL);

	LEDs[led].R = c->R;
	LEDs[led].G = c->G;
	LEDs[led].B = c->B;
}


void setLED_32(uint led, uint32_t rgb){
	assert_param(rgb <= 0xffffff);

	setLED(led, (rgb & 0x00ff0000) >> 16, (rgb & 0x0000ff00) >> 8, (rgb & 0x000000ff) >> 0);
}

void setAllLED(uint8_t r, uint8_t g, uint8_t b){
	for(uint i = 0; i<NR_PIXEL; i++)
	{
		setLED(i, r, g, b);
	}
}

void setAllLED_Color(color *c){
	for(uint i = 0; i<NR_PIXEL; i++)
	{
		setLED_Color(i, c);
	}
}

void setAllLED_32(uint32_t rgb){
	for(uint i = 0; i<NR_PIXEL; i++)
	{
		setLED_32(i, rgb);
	}
}

void clearAllLED(){
	setAllLED(0, 0, 0);
}


// initialization of peripherals for ws2811:
// GPIO, timer (PWM mode), DMA, NVIC
void ws2811_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //TIM_TimeBaseInitTypeDef timbaseinit;
  //TIM_OCInitTypeDef timocinit;
  SPI_InitTypeDef SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  // data init, make sure reset pulse is zero
  int i;
  for (i = 0; i < (WS2811_FRAMEBUF_LED_LEN); i++)
    framebuffer[i] = WS2811_PWM_ZERO;
  for (; i < WS2811_FRAMEBUF_LEN; i++)
    framebuffer[i] = 0;

   // GPIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    SPI_I2S_DeInit(SPI3);
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_Init(SPI3, &SPI_InitStructure);
    SPI_Cmd(SPI3, ENABLE);


  // DMA
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)framebuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = WS2811_FRAMEBUF_LEN;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA1_Stream5, &DMA_InitStructure);



  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


// begin transferring framebuffer data to the timer
void start_dma(void)
{
	if(!DMA_BUSY){
		DMA_Cmd(DMA1_Stream5, ENABLE);
        SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

        DMA_BUSY = TRUE;
	}
}

static inline uint8_t ledBitToOut(uint8_t ledBit){
    if(ledBit)
        return WS2811_PWM_ONE;
    else
        return WS2811_PWM_ZERO;

}

void static inline LED_TO_PWM(uint8_t i) { //i = LED number

    uint8_t outputByte=0;
    uint8_t currentByte = pLEDs[i];
   for(uint8_t b=0; b<BIT_FACTOR;b++){
       uint8_t currentBit = (currentByte >> (b*2)+1) & 0x1;
       outputByte = (ledBitToOut(currentBit) << 4);
       currentBit = (currentByte >> (b*2)) & 0x1;
       outputByte = outputByte | ledBitToOut(currentBit);
       framebuffer[i*BIT_FACTOR+b] = outputByte;

   }
}

void updateLED(void){
	//calculate only the first LED, so the DMA can start while the rest is calculating
	LED_TO_PWM(0);
	//start the Transfer and return
	start_dma();

	//calculate the rest of LEDs
	for (int i = 1; i < (NR_LEDS); i++) {
		LED_TO_PWM(i);
	}
}

// gets called when dma transfer has completed
void DMA1_Stream5_IRQHandler(void)
{
  DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);

	DMA_BUSY = FALSE;

	DMA_Cmd(DMA1_Stream5, DISABLE);
	while (DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5))
		;
}
