#ifndef REGULATION_H_INCLUDED
#define REGULATION_H_INCLUDED

#include "stm32f10x.h"

#define USE_NEURAL 1

#if USE_NEURAL==0
uint16_t Reg_PID();
#else
#define MAXWEIGHT 255
//#define MAXFWD  32767
#define MAX_NEURON_OUT 64

#define INPUT_NEURONS 6
#define HIDDEN_NEURONS 12
#define OUTPUT_NEURONS 2

#define IH_SIZE_IN INPUT_NEURONS
#define IH_SIZE_OUT HIDDEN_NEURONS
#define HO_SIZE_IN HIDDEN_NEURONS
#define HO_SIZE_OUT OUTPUT_NEURONS
#define CH_SIZE HIDDEN_NEURONS
#define CO_SIZE OUTPUT_NEURONS

typedef int32_t tInputType;
typedef int32_t tOutputType;
typedef int16_t tNeuronConn;

typedef enum
{
    INNERN,
    OUTERN
}tNeuronType;

typedef enum
{
    IH_LAYER=0,
    HO_LAYER=1,
    CH_LAYER=2,
    CO_LAYER=3
} tLayerType;

enum {CONN_IH,CONN_HO,CONN_OC,CONN_HC} net_conn;
enum {INNER,OUTER} neur_type;
void Reg_Neural(tOutputType *sensor_in);
void Reg_InitNeural();
void Reg_LayerOutputs(uint8_t type);
int32_t Reg_ActivateNeuron(uint8_t type, int32_t accum);
//void Reg_PrintWeights(tNeuronConn **weights,uint8_t iNum,uint8_t oNum);
//void Reg_PrintWeights(tNeuronConn *weights[],uint8_t iNum,uint8_t oNum);
void Reg_PrintWeights(tNeuronConn weights[][HIDDEN_NEURONS],uint8_t iNum,uint8_t oNum);
void Reg_PrintWeightsAll();
void neuralBrainCycle();
#endif

#endif
