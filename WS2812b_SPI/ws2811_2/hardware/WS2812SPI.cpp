#include "WS2812SPI.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_spi.h"
#include "misc.h"


bool CWS2812SPI::DMA_BUSY = false;
const uint8_t CWS2812SPI::BIT_FACTOR = 4;

const uint8_t CWS2812SPI::lookUp[] = {  (WS2812_PWM_ZERO << BIT_FACTOR) | WS2812_PWM_ZERO,
                                        (WS2812_PWM_ONE << BIT_FACTOR) | WS2812_PWM_ZERO,
                                        (WS2812_PWM_ZERO << BIT_FACTOR) | WS2812_PWM_ONE,

                                        (WS2812_PWM_ONE << BIT_FACTOR) | WS2812_PWM_ONE, };

CWS2812SPI::CWS2812SPI(uint16_t _nr_pixel) : IPixelDisplay(_nr_pixel), WS2812_FRAMEBUF_LED_LEN(NR_LEDS * BIT_FACTOR), WS2812_FRAMEBUF_LEN(WS2812_FRAMEBUF_LED_LEN + WS2812_RESET_LEN)
{
    framebuffer = new uint8_t[WS2812_FRAMEBUF_LEN];
    pPixel = (uint8_t *) IPixelDisplay::Pixel;

}

CWS2812SPI::~CWS2812SPI()
{
    delete[] framebuffer;
}

// initialization of peripherals for ws2811:
// GPIO, timer (PWM mode), DMA, NVIC
void CWS2812SPI::init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // data init, make sure reset pulse is zero
    int i;
    for (i = 0; i < (WS2812_FRAMEBUF_LED_LEN); i++)
        framebuffer[i] = WS2812_PWM_ZERO;
    for (; i < WS2812_FRAMEBUF_LEN; i++)
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

    //SPI init
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


    // DMA init
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)framebuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = WS2812_FRAMEBUF_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA1_Stream5, &DMA_InitStructure);



    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

    //Enable DMA ready interrupt
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_BUSY = false;

}


// begin transferring framebuffer data to the timer
void CWS2812SPI::start_dma(void)
{
    if(!DMA_BUSY)
    {
        DMA_Cmd(DMA1_Stream5, ENABLE);
        SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

        DMA_BUSY = true;
    }
}

inline uint8_t CWS2812SPI::ledBitToOut(uint8_t ledBit)
{
    if(ledBit)
        return WS2812_PWM_ONE;
    else
        return WS2812_PWM_ZERO;

}

inline void CWS2812SPI::LED_TO_PWM(uint8_t i)   //i = LED number
{

    uint8_t outputByte=0;
    uint8_t currentByte = pPixel[i];

    for(uint8_t b=0; b<BIT_FACTOR; b++)
    {
        uint8_t currentBit = (currentByte >> ((BIT_FACTOR-b-1)*2)) & 0x3;
        outputByte = lookUp[currentBit];
        framebuffer[i*BIT_FACTOR+b] = outputByte;
    }
}

bool CWS2812SPI::update(void)
{
    if(!DMA_BUSY)
    {
        //calculate only the first LED, so the DMA can start while the rest is calculating
        LED_TO_PWM(0);

        //start the Transfer and return
        start_dma();

        //calculate the rest of LEDs
        for (uint8_t i = 1; i < (NR_LEDS); i++)
        {
            LED_TO_PWM(i);
        }
        return true;
    }
    else
    {
        return false;
    }

}

void CWS2812SPI::DMAReadyInterrupt(void)
{
    DMA_BUSY = false;
}

extern "C" {
// gets called when dma transfer has completed
    void DMA1_Stream5_IRQHandler(void)
    {
        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
        //set data pin to low to latch  WS2811
        SPI3->DR = 0;

        CWS2812SPI::DMAReadyInterrupt();

        DMA_Cmd(DMA1_Stream5, DISABLE);
        while (DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5))
            ;
    }
}

