#include "sensor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "misc.h"
#include "stm32f10x_dma.h"
#include "string.h"

//static uint16_t sensor_outputs[AD_SENSOR_NUM];
//static uint8_t active_sensor=0;
__IO uint16_t ADCConvertedValue[AD_SENSOR_NUM];

//void ADC1_IRQHandler(void)
//{
//	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)
//	{
//		sensor_outputs[active_sensor]=ADC_GetConversionValue(ADC1);
//		if((++active_sensor) >= AD_SENSOR_NUM)
//			active_sensor=0;
//		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
////		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//	}
//}

void NVIC_configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void Sens_GetLineInput(tOutputType *output)
{
	uint8_t currField;
	//nie je to az taky dobry napad ak mame dve rozdielne velkosti
	//memcpy(output,ADCConvertedValue,AD_SENSOR_NUM*sizeof(tOutputType));
	for(currField=1;currField<AD_SENSOR_NUM;currField++)
	{
		output[currField]=ADCConvertedValue[currField];
	}
	output[0]=275; // tento senzor je pokazeny
}

uint8_t Sens_Get()
{
	return (uint8_t)(GPIO_ReadInputData(GPIOA)&0x003F);
}

void Sens_OutStr(uint8_t raw,unsigned char* str)
{
	uint8_t i,raw2;
	raw2=raw;
	for(i=0;i<6;i++)
	{
		str[i]=(raw2%2)+'0';
		raw>>1;
	}
	str[6]='\r';
	str[7]='\n';
}

void Sens_InitADC()
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	//NVIC_configuration();

	DMA_DeInit(DMA1_Channel1); //Set DMA registers to default values
	DMA_InitStructure.DMA_PeripheralBaseAddr = &(ADC1->DR); //Address of peripheral the DMA must map to
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & ADCConvertedValue; //Variable to which ADC values will be stored
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = AD_SENSOR_NUM; //Buffer size
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //Initialise the DMA
	DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 6;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_55Cycles5);

	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_DMACmd(ADC1, ENABLE); //Enable ADC1 DMA

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
}
