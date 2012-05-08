#ifndef SENSOR_H_INCLUDED
#define SENSOR_H_INCLUDED
#include "stm32f10x.h"
#include "regulation.h"

#define AD_SENSOR_NUM 6

uint8_t Sens_Get();
void Sens_GetLineInput(tOutputType *sensor_in);
void Sens_OutStr(uint8_t raw,unsigned char *str);
void Sens_InitADC();
#endif
