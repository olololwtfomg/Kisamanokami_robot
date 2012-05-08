#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "motor.h"

/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t PrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
//void RCC_Configuration(void);
//void GPIO_Configuration(void);

void Mot_SetSpeed(uint8_t ucMotor, uint8_t ucDirection, uint16_t uiSpeed)
{
	switch (ucMotor)
	{
	case LEFT :
		if(ucDirection==FWD)
		{
			TIM_SetCompare1( TIM3, uiSpeed);
			TIM_SetCompare2( TIM3, OFF);
		}
		else
		{
			TIM_SetCompare1( TIM3, OFF);
			TIM_SetCompare2( TIM3, uiSpeed);
		}
		break;
	case RIGHT:
		if(ucDirection==FWD)
		{
			TIM_SetCompare3( TIM3, uiSpeed);
			TIM_SetCompare4( TIM3, OFF);
		}
		else
		{
			TIM_SetCompare3( TIM3, OFF);
			TIM_SetCompare4( TIM3, uiSpeed);
		}
		break;
	default:
		break;
	}
}

void Mot_InitMotors()
{
	 /* System Clocks Configuration */
	//RCC_Configuration();

	/* GPIO Configuration */
	//GPIO_Configuration();

	/* -----------------------------------------------------------------------
	TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
	The TIM3CLK frequency is set to SystemCoreClock (Hz), to get TIM3 counter
	clock at 24 MHz the Prescaler is computed as following:
	 - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
	and Connectivity line devices and to 24 MHz for Low-Density Value line and
	Medium-Density Value line devices

	The TIM3 is running at 36 KHz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
												  = 24 MHz / 666 = 36 KHz
	TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
	TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
	TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
	TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
	----------------------------------------------------------------------- */
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = MAXFWD;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = MAXFWD;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = OFF;

	TIM_OC2Init(TIM3, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = MAXFWD;

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = OFF;

	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

//void RCC_Configuration(void)
//{
//    /* TIM3 clock enable */
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//
//    /* GPIOA and GPIOB clock enable */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
//                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
//}

//void GPIO_Configuration(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//
//    #ifdef STM32F10X_CL
//        /*GPIOB Configuration: TIM3 channel1, 2, 3 and 4 */
//        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//        GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//        GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
//
//    #else
//        /* GPIOA Configuration:TIM3 Channel1, 2, 3 and 4 as alternate function push-pull */
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_Init(GPIOC, &GPIO_InitStructure);
//        GPIO_PinRemapConfig( GPIO_FullRemap_TIM3, ENABLE );
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOA, &GPIO_InitStructure);
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//		GPIO_Init(GPIOB, &GPIO_InitStructure);
//    #endif
//}
