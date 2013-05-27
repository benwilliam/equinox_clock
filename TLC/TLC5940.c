#include "TLC5940.h"
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#define TLCPORT GPIOA
//#define GSCLK GPIO_Pin_8 //will be done from timer
#define BLANK GPIO_Pin_1
#define VPRG GPIO_Pin_2
#define XLAT GPIO_Pin_3
#define DCPRG GPIO_Pin_4

static volatile uint8_t XLAT_FLAG = 0;
static volatile uint8_t DC_SCLK_FLAG = 0;
static volatile uint8_t GS_DATA_FLAG = 0;
//#define SCLK GPIO_Pin_5 //will be done from SPI2_SCLK
//#define SIN GPIO_Pin_7 //will be done from SPI2
#define pulse(port, pin) do {                       \
                           GPIO_SetBits((port), (pin));  \
                           GPIO_ResetBits((port), (pin));   \
                         } while (0)
gsData_t gsData[gsDataSize];

#if (TLC5940_INCLUDE_GAMMA_CORRECT)
#if (TLC5940_PWM_BITS == 12)
#define V 4095.0
#elif (TLC5940_PWM_BITS == 11)
#define V 2047.0
#elif (TLC5940_PWM_BITS == 10)
#define V 1023.0
#elif (TLC5940_PWM_BITS == 9)
#define V 511.0
#elif (TLC5940_PWM_BITS == 8)
#define V 255.0
#else
#error "TLC5940_PWM_BITS must be 8, 9, 10, 11, or 12"
#endif // TLC5940_PWM_BITS
// Maps a linear 8-bit value to a TLC5940_PWM_BITS-bit gamma corrected value
// This array was computer-generated using the following formula:
// for (uint16_t x = 0; x < 256; x++)
//   printf("%e*V+.5, ", (pow((double)x / 255.0, 2.5)));
uint16_t TLC5940_GammaCorrect[] = {
	0.000000e+00*V+.5, 9.630516e-07*V+.5, 5.447842e-06*V+.5, 1.501249e-05*V+.5,
	3.081765e-05*V+.5, 5.383622e-05*V+.5, 8.492346e-05*V+.5, 1.248518e-04*V+.5,
	1.743310e-04*V+.5, 2.340215e-04*V+.5, 3.045437e-04*V+.5, 3.864838e-04*V+.5,
	4.803996e-04*V+.5, 5.868241e-04*V+.5, 7.062682e-04*V+.5, 8.392236e-04*V+.5,
	9.861648e-04*V+.5, 1.147551e-03*V+.5, 1.323826e-03*V+.5, 1.515422e-03*V+.5,
	1.722759e-03*V+.5, 1.946246e-03*V+.5, 2.186282e-03*V+.5, 2.443257e-03*V+.5,
	2.717551e-03*V+.5, 3.009536e-03*V+.5, 3.319578e-03*V+.5, 3.648035e-03*V+.5,
	3.995256e-03*V+.5, 4.361587e-03*V+.5, 4.747366e-03*V+.5, 5.152925e-03*V+.5,
	5.578591e-03*V+.5, 6.024686e-03*V+.5, 6.491527e-03*V+.5, 6.979425e-03*V+.5,
	7.488689e-03*V+.5, 8.019621e-03*V+.5, 8.572521e-03*V+.5, 9.147682e-03*V+.5,
	9.745397e-03*V+.5, 1.036595e-02*V+.5, 1.100963e-02*V+.5, 1.167672e-02*V+.5,
	1.236748e-02*V+.5, 1.308220e-02*V+.5, 1.382115e-02*V+.5, 1.458459e-02*V+.5,
	1.537279e-02*V+.5, 1.618601e-02*V+.5, 1.702451e-02*V+.5, 1.788854e-02*V+.5,
	1.877837e-02*V+.5, 1.969424e-02*V+.5, 2.063640e-02*V+.5, 2.160510e-02*V+.5,
	2.260058e-02*V+.5, 2.362309e-02*V+.5, 2.467286e-02*V+.5, 2.575014e-02*V+.5,
	2.685516e-02*V+.5, 2.798815e-02*V+.5, 2.914934e-02*V+.5, 3.033898e-02*V+.5,
	3.155727e-02*V+.5, 3.280446e-02*V+.5, 3.408077e-02*V+.5, 3.538641e-02*V+.5,
	3.672162e-02*V+.5, 3.808661e-02*V+.5, 3.948159e-02*V+.5, 4.090679e-02*V+.5,
	4.236242e-02*V+.5, 4.384870e-02*V+.5, 4.536583e-02*V+.5, 4.691403e-02*V+.5,
	4.849350e-02*V+.5, 5.010446e-02*V+.5, 5.174710e-02*V+.5, 5.342165e-02*V+.5,
	5.512829e-02*V+.5, 5.686723e-02*V+.5, 5.863868e-02*V+.5, 6.044283e-02*V+.5,
	6.227988e-02*V+.5, 6.415003e-02*V+.5, 6.605348e-02*V+.5, 6.799041e-02*V+.5,
	6.996104e-02*V+.5, 7.196554e-02*V+.5, 7.400411e-02*V+.5, 7.607694e-02*V+.5,
	7.818422e-02*V+.5, 8.032614e-02*V+.5, 8.250289e-02*V+.5, 8.471466e-02*V+.5,
	8.696162e-02*V+.5, 8.924397e-02*V+.5, 9.156189e-02*V+.5, 9.391556e-02*V+.5,
	9.630516e-02*V+.5, 9.873087e-02*V+.5, 1.011929e-01*V+.5, 1.036914e-01*V+.5,
	1.062265e-01*V+.5, 1.087985e-01*V+.5, 1.114074e-01*V+.5, 1.140536e-01*V+.5,
	1.167371e-01*V+.5, 1.194582e-01*V+.5, 1.222169e-01*V+.5, 1.250135e-01*V+.5,
	1.278482e-01*V+.5, 1.307211e-01*V+.5, 1.336324e-01*V+.5, 1.365822e-01*V+.5,
	1.395708e-01*V+.5, 1.425983e-01*V+.5, 1.456648e-01*V+.5, 1.487705e-01*V+.5,
	1.519157e-01*V+.5, 1.551004e-01*V+.5, 1.583249e-01*V+.5, 1.615892e-01*V+.5,
	1.648936e-01*V+.5, 1.682382e-01*V+.5, 1.716232e-01*V+.5, 1.750487e-01*V+.5,
	1.785149e-01*V+.5, 1.820220e-01*V+.5, 1.855701e-01*V+.5, 1.891593e-01*V+.5,
	1.927899e-01*V+.5, 1.964620e-01*V+.5, 2.001758e-01*V+.5, 2.039313e-01*V+.5,
	2.077289e-01*V+.5, 2.115685e-01*V+.5, 2.154504e-01*V+.5, 2.193747e-01*V+.5,
	2.233416e-01*V+.5, 2.273512e-01*V+.5, 2.314038e-01*V+.5, 2.354993e-01*V+.5,
	2.396381e-01*V+.5, 2.438201e-01*V+.5, 2.480457e-01*V+.5, 2.523149e-01*V+.5,
	2.566279e-01*V+.5, 2.609848e-01*V+.5, 2.653858e-01*V+.5, 2.698310e-01*V+.5,
	2.743207e-01*V+.5, 2.788548e-01*V+.5, 2.834336e-01*V+.5, 2.880572e-01*V+.5,
	2.927258e-01*V+.5, 2.974395e-01*V+.5, 3.021985e-01*V+.5, 3.070028e-01*V+.5,
	3.118527e-01*V+.5, 3.167483e-01*V+.5, 3.216896e-01*V+.5, 3.266770e-01*V+.5,
	3.317105e-01*V+.5, 3.367902e-01*V+.5, 3.419163e-01*V+.5, 3.470889e-01*V+.5,
	3.523082e-01*V+.5, 3.575743e-01*V+.5, 3.628874e-01*V+.5, 3.682475e-01*V+.5,
	3.736549e-01*V+.5, 3.791096e-01*V+.5, 3.846119e-01*V+.5, 3.901617e-01*V+.5,
	3.957594e-01*V+.5, 4.014049e-01*V+.5, 4.070985e-01*V+.5, 4.128403e-01*V+.5,
	4.186304e-01*V+.5, 4.244690e-01*V+.5, 4.303562e-01*V+.5, 4.362920e-01*V+.5,
	4.422767e-01*V+.5, 4.483105e-01*V+.5, 4.543933e-01*V+.5, 4.605254e-01*V+.5,
	4.667068e-01*V+.5, 4.729378e-01*V+.5, 4.792185e-01*V+.5, 4.855489e-01*V+.5,
	4.919292e-01*V+.5, 4.983596e-01*V+.5, 5.048401e-01*V+.5, 5.113710e-01*V+.5,
	5.179523e-01*V+.5, 5.245841e-01*V+.5, 5.312666e-01*V+.5, 5.380000e-01*V+.5,
	5.447842e-01*V+.5, 5.516196e-01*V+.5, 5.585062e-01*V+.5, 5.654441e-01*V+.5,
	5.724334e-01*V+.5, 5.794743e-01*V+.5, 5.865670e-01*V+.5, 5.937114e-01*V+.5,
	6.009079e-01*V+.5, 6.081564e-01*V+.5, 6.154571e-01*V+.5, 6.228102e-01*V+.5,
	6.302157e-01*V+.5, 6.376738e-01*V+.5, 6.451846e-01*V+.5, 6.527482e-01*V+.5,
	6.603648e-01*V+.5, 6.680345e-01*V+.5, 6.757574e-01*V+.5, 6.835336e-01*V+.5,
	6.913632e-01*V+.5, 6.992464e-01*V+.5, 7.071833e-01*V+.5, 7.151740e-01*V+.5,
	7.232186e-01*V+.5, 7.313173e-01*V+.5, 7.394701e-01*V+.5, 7.476773e-01*V+.5,
	7.559389e-01*V+.5, 7.642549e-01*V+.5, 7.726257e-01*V+.5, 7.810512e-01*V+.5,
	7.895316e-01*V+.5, 7.980670e-01*V+.5, 8.066575e-01*V+.5, 8.153033e-01*V+.5,
	8.240044e-01*V+.5, 8.327611e-01*V+.5, 8.415733e-01*V+.5, 8.504412e-01*V+.5,
	8.593650e-01*V+.5, 8.683447e-01*V+.5, 8.773805e-01*V+.5, 8.864724e-01*V+.5,
	8.956207e-01*V+.5, 9.048254e-01*V+.5, 9.140865e-01*V+.5, 9.234044e-01*V+.5,
	9.327790e-01*V+.5, 9.422104e-01*V+.5, 9.516989e-01*V+.5, 9.612445e-01*V+.5,
	9.708472e-01*V+.5, 9.805073e-01*V+.5, 9.902249e-01*V+.5, 1.000000e+00*V+.5,
};
#undef V
#endif // TLC5940_INCLUDE_GAMMA_CORRECT
#if (TLC5940_INCLUDE_DC_FUNCS)
dcData_t dcData[dcDataSize];

void TLC5940_SetDC(channel_t channel, uint8_t value) {
	channel = numChannels - 1 - channel;
	channel_t i = (channel3_t)channel * 3 / 4;

	switch (channel % 4) {
		case 0:
		dcData[i] = (dcData[i] & 0x03) | (uint8_t)(value << 2);
		break;
		case 1:
		dcData[i] = (dcData[i] & 0xFC) | (value >> 4);
		i++;
		dcData[i] = (dcData[i] & 0x0F) | (uint8_t)(value << 4);
		break;
		case 2:
		dcData[i] = (dcData[i] & 0xF0) | (value >> 2);
		i++;
		dcData[i] = (dcData[i] & 0x3F) | (uint8_t)(value << 6);
		break;
		default: // case 3:
		dcData[i] = (dcData[i] & 0xC0) | (value);
		break;
	}
}

void TLC5940_SetAllDC(uint8_t value) {
	uint8_t tmp1 = (uint8_t)(value << 2);
	uint8_t tmp2 = (uint8_t)(tmp1 << 2);
	uint8_t tmp3 = (uint8_t)(tmp2 << 2);
	tmp1 |= (value >> 4);
	tmp2 |= (value >> 2);
	tmp3 |= value;

	dcData_t i = 0;
	do {
		dcData[i++] = tmp1; // bits: 05 04 03 02 01 00 05 04
		dcData[i++] = tmp2;// bits: 03 02 01 00 05 04 03 02
		dcData[i++] = tmp3;// bits: 01 00 05 04 03 02 01 00
	}while (i < dcDataSize);
}
#endif

//#define SOFTSPIPORT GPIOB
//#define SCLK GPIO_Pin_13
//#define MOSI GPIO_Pin_15
////debug propose
void shortdelay(void) {
	for (uint32_t i = 0; i < 100; i++) {

	}
}
//void clockindata(void) {
//	pulse(SOFTSPIPORT, SCLK);
//	for (int8_t i = gsDataSize-1; i > 0; i--) {
//		for (int8_t b = 7; b > 0; b--) {
//			uint8_t temp = gsData[i] >> b;
//			temp = temp & 0x1;
//			if(temp)
//				GPIO_SetBits(SOFTSPIPORT, MOSI);
//			else
//				GPIO_ResetBits(SOFTSPIPORT, MOSI);
//			shortdelay();
//			GPIO_SetBits(SOFTSPIPORT, SCLK);
//			shortdelay();
//			GPIO_ResetBits(SOFTSPIPORT, SCLK);
//		}
//	}
//	GPIO_ResetBits(SOFTSPIPORT, MOSI);
//	XLAT_FLAG = ENABLE;
//}
//
//
//void initSoftSPI(void) {
//	GPIO_InitTypeDef GPIO_InitStructure;
//	/* Enable GPIO clocks */
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	GPIO_ResetBits(SOFTSPIPORT, GPIO_Pin_13);
//	GPIO_ResetBits(SOFTSPIPORT, GPIO_Pin_15);
//}

void TLC5940_UpdateGS(void){
	GS_DATA_FLAG = ENABLE;
}
void TLC5940_SendGS_DMARequest(void) {
	//DMA_Cmd(DMA1_Stream4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Stream4, gsDataSize);
	DMA_Cmd(DMA1_Stream4, ENABLE);
//	clockindata();
}

void DMA1_Stream4_IRQHandler(void) {
	DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
	//wait until flag is deletet
	XLAT_FLAG = ENABLE;
	GS_DATA_FLAG = DISABLE;
	while(DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4));
}

void TIM2_IRQHandler(void) {
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	GPIO_SetBits(TLCPORT, BLANK);
	if (XLAT_FLAG) {
		//pulse(TLCPORT, XLAT);
		//deinitMCO();
		GPIO_SetBits(TLCPORT, XLAT);
		GPIO_ResetBits(TLCPORT, XLAT);
		//initMCO();
		XLAT_FLAG = 0;
	}
	GPIO_ResetBits(TLCPORT, BLANK);
	if(GS_DATA_FLAG)
	{
		TLC5940_SendGS_DMARequest();
	}
}

/**
 * @brief  Init GPIOs  for BLANK | VPRG | XLAT | DCPRG
 *
 */
void initGPIO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = BLANK | VPRG | XLAT | DCPRG;
	GPIO_Init(TLCPORT, &GPIO_InitStructure);
}

/**
 * @brief  Init Timer2  for BLANK
 *
 */
void initTimer(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 2;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 4095;
	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBase_InitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	// Timer Interrupt after 4096 -> toggle blank
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&NVIC_InitStructure);

	// Timer starten
	TIM_Cmd(TIM2, ENABLE);
}

void deinitMCO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(TLCPORT, &GPIO_InitStructure);

	GPIO_ResetBits(TLCPORT, GPIO_Pin_8);
}
/**
 * @brief  Init MCO0 for GSCLK
 *
 */
void initMCO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(TLCPORT, &GPIO_InitStructure);

	RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_4);
}

/**
 * @brief  Init SPI2  with GPIOs and DMA
 *
 */
void initSPI(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Enable DMA clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2); //SCK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2); //MOSI

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_I2S_DeInit(SPI2);
	SPI_I2S_DeInit(SPI1);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI2, ENABLE);
}

/**
 * @brief  Init DMA for SPI2
 *
 */
void initDMA(void) {
	DMA_InitTypeDef DMA_InitStructure;
	// DMA Channel 0 - SPI TX
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & SPI2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) gsData;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = gsDataSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //immer nur Byteweise
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//don't start the transmission yet
	//  DMA_Cmd(DMA1_Stream4, ENABLE);
}

void TLC5940_Init(void) {
	initGPIO();
	GPIO_SetBits(TLCPORT, BLANK);
	GPIO_SetBits(TLCPORT, VPRG);
	GPIO_ResetBits(TLCPORT, XLAT);
	GPIO_ResetBits(TLCPORT, DCPRG);
	GPIO_ResetBits(TLCPORT, VPRG);

	initSPI();
	initDMA();
	TLC5940_SendGS_DMARequest();

//	initSoftSPI();

	initMCO();
	initTimer();

}

void TLC5940_SetGS(channel_t channel, uint16_t value) {
	channel = numChannels - 1 - channel;
	channel3_t i = (channel3_t) channel * 3 / 2;

	switch (channel % 2) {
	case 0:
		gsData[i++] = (value >> 4);
		gsData[i] = (gsData[i] & 0x0F) | (uint8_t)(value << 4);
		break;
	default: // case 1:
		gsData[i] = (gsData[i] & 0xF0) | (value >> 8);
		gsData[++i] = (uint8_t) value;
		break;
	}
}

void TLC5940_SetAllGS(uint16_t value) {
	uint8_t tmp1 = (value >> 4);
	uint8_t tmp2 = (uint8_t)(value << 4) | (tmp1 >> 4);

	gsData_t i = 0;
	do {
		gsData[i++] = tmp1; // bits: 11 10 09 08 07 06 05 04
		gsData[i++] = tmp2; // bits: 03 02 01 00 11 10 09 08
		gsData[i++] = (uint8_t) value; // bits: 07 06 05 04 03 02 01 00
	} while (i < gsDataSize);
}
