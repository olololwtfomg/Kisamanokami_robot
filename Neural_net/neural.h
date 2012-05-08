/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <olololwtfomg@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Jaroslav Mucka
 * ----------------------------------------------------------------------------
 */

#ifndef NEURAL_H_INCLUDED
#define NEURAL_H_INCLUDED

#include "genetics.h"
#include <stdio.h>
#include <stdint.h>

#define MAXWEIGHT 255
#define MAXFWD  32767
#define MAX_NEURON_OUT 64

#define INPUT_NEURONS 6
#define HIDDEN_NEURONS 12
#define OUTPUT_NEURONS 2

#define STEPS 3

enum {CONN_IH,CONN_HO,CONN_OC,CONN_HC} net_conn;
enum {INNER,OUTER} neur_type;
void Reg_Neural(unsigned short *sensor_in,unsigned char steps);
void Reg_InitNeural();
void Reg_LayerOutputs(unsigned char type,unsigned char steps);
int Reg_ActivateNeuron(unsigned char type, int accum);
void Reg_TeachNeurons();

void Reg_PrintWeightsAll();
void Reg_PrintOutputs();

#endif // NEURAL_H_INCLUDED
