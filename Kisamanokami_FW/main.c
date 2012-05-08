#include "motor.h"
#include "Gpio_setup.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
#include "sensor.h"
#include "regulation.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
	const char menu[] = " Welcome to HELL!\r\n";
	tOutputType sensor_in[AD_SENSOR_NUM];
	//unsigned char sens_out[8];
	char tmpS[200];
	Setup_HW();
	Mot_InitMotors();
	//Reg_InitNeural();
	//Reg_PrintWeightsAll();
	uint32_t i=0;

	GPIO_SetBits(GPIOA, GPIO_Pin_8|GPIO_Pin_9);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);

	usart_rxtx();
	UARTSend(menu, strlen(menu));
	Reg_PrintWeightsAll();
	while(1)
    {
		USART_Reply();
		//__WFI();
//		Mot_SetSpeed(LEFT, RWD,(uint16_t) MAXFWD);
//		Mot_SetSpeed(RIGHT, RWD,(uint16_t) MAXFWD);
		for(i=0;i<0x5000;i++)
		{
			asm volatile("nop");
		}
		Sens_GetLineInput(sensor_in);
		sprintf(tmpS,"1:%6d 2:%6d 3:%6d 4:%6d 5:%6d 6:%6d\r\n",sensor_in[0],sensor_in[1],
				sensor_in[2],sensor_in[3],sensor_in[4],sensor_in[5]);
		UARTSend(tmpS, strlen(tmpS));
		//Reg_PrintWeightsAll();
		//neuralBrainCycle();
		Reg_Neural(sensor_in);

//		Mot_SetSpeed(LEFT, RWD, MAXFWD);
//		Mot_SetSpeed(RIGHT, FWD, MAXFWD);
//		for(i=0;i<0x50000000;i++);
//		Mot_SetSpeed(LEFT, RWD, MAXFWD);
//		Mot_SetSpeed(RIGHT, RWD, MAXFWD);
//		for(i=0;i<0x100000;i++);
//		Mot_SetSpeed(LEFT, FWD, MAXFWD);
//	    Mot_SetSpeed(RIGHT, RWD, MAXFWD);
//	    for(i=0;i<0x100000;i++);
    }
}

