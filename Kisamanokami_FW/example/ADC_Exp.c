#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#define LED_PORT GPIOB
#define ADC1_DR_Address    ((u32)0x4001244C)

/***************************************************************************//**
 * Global variables
 ******************************************************************************/
int AD_value;
static unsigned long ticks;
unsigned char Clock1s;
ADC_InitTypeDef ADC_InitStructure;
vu16 ADC_ConvertedValue;
ErrorStatus HSEStartUpStatus;

/***************************************************************************//**
 * Declare function prototype
 ******************************************************************************/
void Delay(__IO uint32_t nCount);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

/***************************************************************************//**
 * @brief  This example describes how to use the ADC to converts continuously
 *         Analog signal to digital signal through channel1.
 ******************************************************************************/
int ADC_Exp(void)
{
    /* System clocks configuration ---------------------------------------------*/
    RCC_Configuration();

    /* NVIC configuration ------------------------------------------------------*/
    NVIC_Configuration();

    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();

    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel14 configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Enable ADC1 reset calibaration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while (ADC_GetResetCalibrationStatus(ADC1));

    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1));

    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (1)
    {
        AD_value = ADC_GetConversionValue(ADC1);
        if (ticks++ >= 9999)
        { /* Set Clock1s to 1 every 1 second    */
            ticks = 0;
            Clock1s = 1;
        }

        /* Printf message with AD value to serial port every 1 second             */
        if (Clock1s)
        {
            Clock1s = 0;
            Delay(AD_value);
            GPIO_WriteBit(LED_PORT, GPIO_Pin_8, Bit_SET);
            Delay(AD_value);
            GPIO_WriteBit(LED_PORT, GPIO_Pin_8, Bit_RESET);
        }
    }
}

/***************************************************************************//**
 * @brief  Configures the different system clocks.
 ******************************************************************************/
void RCC_Configuration(void)
{
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* ADCCLK = PCLK2/4 */
        RCC_ADCCLKConfig(RCC_PCLK2_Div4);

        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* PLLCLK = 8MHz * 9 = 56 MHz */
        RCC_PLLConfig(0x00010000, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd(ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }

    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA
            | RCC_APB2Periph_GPIOB, ENABLE);
}

/***************************************************************************//**
 * @brief  Configures the different GPIO ports.
 ******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure PA.01 (ADC Channel1) as analog input -------------------------*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Configures Vector Table base location.
 ******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

/***************************************************************************//**
 * @brief  Inserts a delay time.
 * @param  nCount: specifies the delay time length.
 ******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    uint32_t i = 0;
    for (; nCount != 0; i++)
    {
        if (i == 1000)
        {
            i = 0;
            nCount--;
        }
    }
}
