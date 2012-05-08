/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <olololwtfomg@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Jaroslav Mucka
 * ----------------------------------------------------------------------------
 */

#ifndef GENETICS_H_INCLUDED
#define GENETICS_H_INCLUDED

#include <stdint.h>
#include "neural.h"

#define CONST1 1/3
#define CONST2 1/3
#define CONST3 1/3

#define MUTATION_PROB 128
#define MAX_REPEAT 100
#define KONTROLA 16

#define IH_SIZE_IN INPUT_NEURONS
#define IH_SIZE_OUT HIDDEN_NEURONS
#define HO_SIZE_IN HIDDEN_NEURONS
#define HO_SIZE_OUT OUTPUT_NEURONS
#define CH_SIZE HIDDEN_NEURONS
#define CO_SIZE OUTPUT_NEURONS


#define JEDINEC_NUM 10000    //mal by byt viac ako 1

#define TESTS_TOTAL 100
#define NOISE_PROB 10

#define OUTPUT_FILE "Weights.txt"

#define TESTCASE_FILE "testcases.txt"

typedef enum
{
    IH_LAYER=0,
    HO_LAYER=1,
    CH_LAYER=2,
    CO_LAYER=3
} tLayerType;

typedef int32_t tInputType;
typedef int32_t tOutputType;

typedef uint64_t tErrorType;
typedef int16_t tNeuronConn;

typedef struct
{
    tInputType *inputs;
    tOutputType *outputs;
}tTestCase;

typedef struct
{
    tNeuronConn **weightIH;
    tNeuronConn **weightHO;
    tNeuronConn **weightCH;
    tNeuronConn **weightCO;
    tNeuronConn **weightHC;
    tNeuronConn **weightOC;
    tOutputType **layerOutputs;
    tOutputType **realOutputs;
    tErrorType avgFinalError;   //average final error
    tErrorType avgI2;           //average abs error sum
    tErrorType avgI3;           //average error sigma
    uint32_t UID;
}
tJedinec;

typedef enum
{
    INNERN,
    OUTERN
}tNeuronType;


void initTestCases();
void holocaust();
void initJedinci(uint32_t num);
void Reg_InitNeuralGenetic(tJedinec *jedinec);
void testFunc();
void generationFermentation();
#endif // GENETICS_H_INCLUDED
