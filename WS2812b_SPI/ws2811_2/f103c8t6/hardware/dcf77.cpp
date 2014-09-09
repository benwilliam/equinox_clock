#include "dcf77.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include <stdio.h>
CDCF77Decoder *CDCF77Decoder::hadnleBitISRInstance;

CDCF77Decoder::CDCF77Decoder()
{
    hadnleBitISRInstance = this;
}

CDCF77Decoder::EBitKind CDCF77Decoder::parseBit(uint8_t measured[])
{
    EBitKind ret = BIT_INVALID;

    bool isBIT_0 = true;
    bool isBIT_1 = true;
    bool isBIT_M = true;

    for (uint8_t i = 0; i < MEASURED_SECOND_SIZE; i++ )
    {
        //try to parse low bit
        if(i >=0 && i<3) // first 4 measured values should be 0 (-1 tolerance)
        {
            if(measured[i] == 1)
            {
                isBIT_0 = false;
            }

        }
        else if (i >= 5 && i<MEASURED_SECOND_SIZE)// last 36 measured values should be 1 (+1 tolereance)
        {
            if(measured[i] == 0)
            {
                isBIT_0 = false;
            }
        }

        //try to parse high bit
        if(i >=0 && i<7) // first 8 measured values should be 0 (-1 tolerance)
        {
            if(measured[i] == 1)
            {
                isBIT_1 = false;
            }
        }
        else if (i >= 9 && i<MEASURED_SECOND_SIZE) // last 32 measured values should be 1 (+1 tolereance)
        {
            if(measured[i] == 0)
            {
                isBIT_1 = false;
            }
        }

        //try to parse minute bit
        if(measured[i] == 0) // all measured values should be 1
        {
            isBIT_M = false;
        }
        printf("%d", measured[i]);
    }

    if(isBIT_0 && !isBIT_1 && !isBIT_M)
    {
        ret = BIT_0;
    }
    else if(isBIT_1 && !isBIT_0 && !isBIT_M)
    {
        ret = BIT_1;
    }
    else if(isBIT_M && !isBIT_0 && !isBIT_1)
    {
        ret = BIT_M;
    }
    printf("\n");
    return ret;
}

void CDCF77Decoder::handleMeassuredSecond(uint8_t measured[])
{
        EBitKind bitValue = parseBit(measured);
}

void CDCF77Decoder::handleBitISR(uint8_t bitValue)
{


    //first sync to bit -> wait for first low measurement
    if(!SyncedToBit)
    {
        if(lastMeasured == 1 && bitValue == 0)
        {
            //first low measured
            SyncedToBit = true;
            measurePos = 0;
            printf("synced to bit\n");
        }
    }

    if(SyncedToBit)
    {
        measuredSecond[measurePos]= bitValue;
        measurePos++;

        if(measurePos >= MEASURED_SECOND_SIZE)
        {
            printf("%d measurements done\n", measurePos);
            measurePos = 0;
            handleMeassuredSecond(measuredSecond);

        }

    }

    lastMeasured = bitValue;
}

void CDCF77Decoder::init()
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // GPIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //enable clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 25-1; // timer set to 1KHz -> we want 40 Hz -> 25ms
    TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/4)/1000)-1; //systemCoreClock(168Mhz) / timerdiv(2) / clock_div(4) / 1KHz(1000) = 21000
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM13, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM13, ENABLE);

}

extern "C" {
    void TIM8_UP_TIM13_IRQHandler()
    {
        if(TIM_GetITStatus(TIM13, TIM_IT_Update))
        {
            //clear pending interrupt
            TIM_ClearITPendingBit(TIM13, TIM_IT_Update);

            uint8_t signal = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);
            printf("%d\n", signal);

            //do here checking for dcf77 signal
            CDCF77Decoder::hadnleBitISRInstance->handleBitISR(signal);
        }
    }
}
