#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include "stm32f10x.h"

#define MAXFWD ((int16_t) 32767)
#define MAXRWD ((int16_t) -32767)
#define OFF		0

enum{FWD=0,RWD=1}eDirection;
enum{LEFT=0,RIGHT=1}eMotor;
enum{INSTANT,SMOOTH,ABS,BACKWARD}eBrake;


void Mot_InitMotors();
void Mot_SetSpeed(uint8_t ucMotor, uint8_t ucDirection, uint16_t uiSpeed);
void Mot_Brake(uint8_t type);
void Mot_DefaultSpeed();

#endif
