#include "regulation.h"
#include "sensor.h"
#include "motor.h"
#include "usart.h"
#include "stm32f10x.h"
#include "fixedpt.h"
#include <stdio.h>
#include <string.h>

char tmpS[200];

typedef tNeuronConn tNeuronRow[HIDDEN_NEURONS];

int32_t rand1(int lim);
int32_t abs(int32_t num);
tOutputType ActivateNeuron(tNeuronType type, tOutputType accum);
void outputActivation(tOutputType *vector, uint32_t len,tNeuronType type);
void calcMatrixOutput(tOutputType *inputs,tOutputType *outputs,tNeuronConn weights[][HIDDEN_NEURONS],uint32_t iNum, uint32_t oNum);
void printOutputVector(tOutputType *output,uint8_t oNum);

int32_t rand1(int lim)
{
	static long a = 100001;
	a = (a * 125) % 2796203;
	a=(a % lim) + 1;
	if(a&(1<<7))
	{
		return a;
	}
	else
	{
		return a*(-1);
	}
}

#if USE_NEURAL==1

//tNeuronConn weightIH[IH_SIZE_IN][IH_SIZE_OUT]
tNeuronConn weightIH[][IH_SIZE_OUT]={

		{   358,   111,  -156,  -119,   299,   193,   -48,  -608,    18,   212,   132,   -76,},

		{  -156,   -71,  -388,  -231,  -235,  -266,   -47,  -476,   128,   -33, -1565,  -322,},

		{   183,   248,  -406,  -177,   202,  -356,   185, -1059,    47,   -60,   -35,     8,},

		{  -121,   196,    40,   202,  -116,   -16,   135,  -976,   -34,    69,  -265,     7,},

		{  -239,   -77,    96,   186,    90,    62,    54,  -603,    85,  -127,  -210,  -228,},

		{  -173,   -67,   644,     3,  -162,  -205,   -44,  -760,    22,   -84,  -627,   235,}

		};
/*
={{266,105,-23,-97,-399,-718,179,-909,-1096,109,-415,7},
		{132,15,-1146,-26,-371,-946,-242,-212,-120,-362,252,174},
		{-107,-287,-570,-391,-146,-602,-113,-436,-376,-169,69,-39},
		{114,-222,-28,-144,-329,-490,215,-515,-1089,-135,-544,173},
		{3,3,119,-357,-1204,-265,-114,-444,386,118,399,-36},
		{528,-196,-255,6,41,-601,-97,-510,11,-210,-59,-49}};
*/
tNeuronConn weightHO[HO_SIZE_IN][HO_SIZE_OUT]=
{

{  -333,  1056,},

{    49,    73,},

{  -587,  -724,},

{   167,  -149,},

{   174,   544,},

{  -129,    23,},

{  -216,  -207,},

{  -206,  -404,},

{   371,   791,},

{  -526,  -392,},

{  -278,  -304,},

{    83,  -594,}

};
/*{{-1348,-989},
		{89,1413},
		{353,-992},
		{-2608,-731},
		{267,46},
		{-3554,-239},
		{-212,-386},
		{986,762},
		{-4857,-51},
		{-4708,-219},
		{-240,396},
		{-956,371}};
		*/

tNeuronConn weightCO[CO_SIZE][CO_SIZE]=
{

{  -992,    -6,},

{   -66,  -236,}

};
/*{{-1835,-1695},
		{508,-2001}};
		*/
tNeuronConn weightCH[CH_SIZE][CH_SIZE]=
{

{  -205,   260,  -360,   340,  -707,  -230,   107,  -538,  -323,   217,  -712,     3,},

{    10,   586,  -789,    90,   -76,  -177,    42,   -25,   430,  -314,  -772,  -245,},

{  -133, -1209,  -733,  -606,  -371,  -253,   -85,  -416,  -263,  -567,   -37,  -130,},

{  -311,   436, -1519,   221,  -306,  -569,  -188,  -289,   103,  -285, -1452,    -5,},

{  -304,   348,  -558,   347,  -272, -1110,  -402,  -874,  -356,  -221,  -277,  -195,},

{    20,  -679,  -955,    85,   -39,   -55,  -477,  -507,  -484,  -230,   408,  -413,},

{   176,  -531,  -952,   194,   638,  -423,   -83, -1321,  -585,    15,  -663,  -196,},

{    52,  -261,  -407,  -443,   335,  -546,  -209,  -294,  -244,    99,  -930,   179,},

{   -72,    87,  -353,  -496,  -284,  -235,  -223,  -501,  -443,  -252,  -432,  -208,},

{  -301,  -704,   218,    34,     4,  -358,  -559,  -633,  -266,   -61, -1001,  -602,},

{  -234,  -594,   -68,  -212,    42,  -810,    76,   178,  -321,  -597,  -389,  -565,},

{  -101,  -159,  -100,    80,  -585,  -668,  -559,  -497,  -700,  -284,  -481,   379,}

};
/*{{40,18,18,-105,-515,-256,150,-772,-304,-227,-275,-178},
		{-173,85,-730,-395,-79,-190,7,-922,-512,16,-174,228},
		{-413,-53,3,-40,-308,-424,145,-665,-628,-107,-79,51},
		{-210,-70,-238,373,393,-620,-44,-865,-845,119,-782,-104},
		{-160,-132,-377,71,-916,-631,18,-161,-783,-94,-56,-207},
		{-72,125,-348,201,-134,-538,-173,-1571,-534,3,278,173},
		{-92,367,14,246,88,280,-309,25,172,25,57,-132},
		{-306,57,249,-50,-511,-58,166,-158,-165,66,-62,55},
		{-290,-76,-14,-111,451,-268,-217,469,-451,-90,-239,1},
		{107,-328,261,198,-998,-203,222,32,349,216,-272,-176},
		{233,35,54,49,-87,-102,58,-68,-106,-152,-43,251},
		{-141,193,-94,-110,-231,125,212,-641,-90,39,-327,179}};
*/
tNeuronConn weightOC[CO_SIZE][CO_SIZE]=
{

{  -160,   446,},

{    98,   127,},

};
	/*{{-354,-1521},
		{-1950,-2885}};
		*/

tNeuronConn weightHC[CH_SIZE][CH_SIZE]=
{

{  -576,  -339,  -176,   389,   215,    12,  -246,  -364, -1055,  -301,   -81,   -83,},

{   177,   352,   441,  -133,   -91,   260,   -31,    43,    14,  -767,   173,  -357,},

{  -598,  -340,  -192,    31,  -194,  -435,  -200,  -535,  -333,  -160,  -104,    88,},

{   -35,   140,    80,    23,    50,    62,  -222,    -2,    65,  -101,   162,   -14,},

{ -1030,  -292,  -443,  -215,  -121,  -201,  -150,  -524,  -201,  -579,   -99,  -388,},

{  -648,   -56,   198,  -252,  -482,   -22,     9,   196,  -672,  -224,   130,  -155,},

{  -590,    14,   -98,   -70,  -241,  -272,  -279,   230,   -84,   -18,   -12,  -225,},

{  -824,  -212,   101,   -21,    53,    -4,   260,  -202,  -825,  -489,  -220,   -54,},

{  -572,  -178,  -212,   106,   -59,    54,    58,  -404,  -145,  -920,  -347,  -146,},

{  -641,  -723,   268,  -102,   -43,  -385,  -343,   -60,  -300, -1032,  -384,   219,},

{  -646,  -305,   234,  -144,   -39,   238,  -176,   268,    31,  -529,   132,   -44,},

{  -677,  -343,   240,  -117,   609,  -123,  -120,    54,  -322,  -174,  -345,  -171,},

};
		/*
{{-1465,-2423,-1999,-1760,-1497,-2658,-2092,-1445,-2071,-1994,-1247,-1059},
		{-1959,-1620,-869,-1041,-2031,-1069,-2472,-1033,-1712,-804,-759,-2357},
		{-2133,-788,-3574,-1049,-1718,-893,-2497,-2048,-1534,-1221,-1462,-922},
		{-1006,-1681,-1463,-2228,122,-1402,-2452,-2102,-1886,-2153,-1129,-1050},
		{-944,-2069,-2995,-3128,-853,-2271,-2267,-913,-2900,-259,-806,-2196},
		{-2644,-1792,-1275,-687,-1756,-1126,-562,-2200,-3258,-2174,-1933,-582},
		{-1182,-2204,-385,-1822,-970,-3079,-1661,-2520,-1619,-1990,-1991,-1455},
		{-2668,-1245,-1594,-1816,-1246,308,-1289,-1372,-1719,255,-306,-1576},
		{-1644,-1472,-2008,-2382,-2205,-1275,-1472,-739,-1748,-906,-2323,-2103},
		{-2378,-1953,-1724,-1163,-1850,-845,-944,-1045,-2351,-2157,-965,-1066},
		{-291,-830,-1477,-1502,-649,-1108,-1822,-2186,-2033,-1993,-1954,-639},
		{-615,-1533,-1994,-1542,-1031,-2186,-2060,-1522,-2129,-2782,-2303,-956}};
		*/

static tOutputType input_outputs[INPUT_NEURONS]={0,};
static tOutputType hidden_outputs[HIDDEN_NEURONS]={0,};
static tOutputType output_outputs[OUTPUT_NEURONS]={0,};
static tOutputType CH_outputs[HIDDEN_NEURONS]={0,};
static tOutputType CO_outputs[OUTPUT_NEURONS]={0,};


void calcMatrixOutput(tOutputType *inputs,tOutputType *outputs,tNeuronConn weights[][HIDDEN_NEURONS],uint32_t iNum, uint32_t oNum)
{
    uint32_t xAxis,yAxis;
    for(yAxis=0;yAxis<iNum;yAxis++)
    {
        for(xAxis=0;xAxis<oNum;xAxis++)
        {
            outputs[xAxis]+=inputs[yAxis]*weights[yAxis][xAxis];
        }
    }
}

tOutputType ActivateNeuron(tNeuronType type, tOutputType accum)
{
    fixedpt fpAccum,fpMaxfwd,tmp;
    tmp=fixedpt_fromint(0);
    if(type==INNERN)
    {
        fpMaxfwd=fixedpt_fromint(MAX_NEURON_OUT);
    }
    else
    {
        fpMaxfwd=fixedpt_fromint(MAXFWD);
    }
    if((accum<FIXED_POS_LIMIT)&&(accum>FIXED_NEG_LIMIT))
    {
        fpAccum=fixedpt_fromint(accum);
    }
    else
    {
        if(accum>FIXED_POS_LIMIT)
        {
            if(type==INNERN)
            {
                fpAccum=fixedpt_fromint((FIXED_POS_LIMIT-1));
            }
            else
            {
                fpAccum=fixedpt_fromint((FIXED_POS_LIMIT-MAXFWD));
            }
        }
        else
        {
            if(type==INNERN)
            {
                fpAccum=fixedpt_fromint((FIXED_POS_LIMIT+1));
            }
            else
            {
                fpAccum=fixedpt_fromint((FIXED_POS_LIMIT+MAXFWD));
            }
        }

    }
	switch (type)
	{
	case INNERN:
        tmp=fixedpt_abs(fpAccum);
		tmp=fixedpt_add(tmp,FIXEDPT_ONE);
		tmp=fixedpt_div(fpAccum,tmp);
		tmp=fixedpt_mul(tmp,fpMaxfwd);
		break;
	case OUTERN:
        tmp=fixedpt_abs(fpAccum);
		tmp=fixedpt_add(tmp,fpMaxfwd);
		tmp=fixedpt_div(fpAccum,tmp);
		tmp=fixedpt_mul(tmp,fpMaxfwd);
		break;
	}
	return fixedpt_toint(tmp);
}

void outputActivation(tOutputType *vector, uint32_t len,tNeuronType type)
{
    uint32_t currOutput;
    for(currOutput=0;currOutput<len;currOutput++)
    {
        vector[currOutput]=ActivateNeuron(type,vector[currOutput]);
    }
}

void neuralBrainCycle()
{
	uint8_t currLayer;
	uint32_t outNum[]={IH_SIZE_OUT,HO_SIZE_OUT,CH_SIZE,CO_SIZE};
	tOutputType *inputsTmp;
	tOutputType tmpOutputs[CH_SIZE]={0,};
	tOutputType *outputs;
	//tNeuronRow *weights;
	//tNeuronConn **weights;
	//tNeuronConn ***weights;
	uint32_t iNum;
	uint32_t oNum;
	uint32_t currOutput;
	for(currLayer=IH_LAYER;currLayer<(CO_LAYER+1);currLayer++)
	{
		//tieto dve vrstvy potrebuju este pridat vstupy z kontextovych neuronov
		if(currLayer<CH_LAYER)
		{
			oNum=outNum[currLayer];
			if(currLayer==IH_LAYER)
			{
				inputsTmp=input_outputs;
				iNum=IH_SIZE_IN;
				//weights=&weightIH;
				outputs=hidden_outputs;
				calcMatrixOutput(inputsTmp,outputs,weightIH,iNum,oNum);
			}
			else
			{
				inputsTmp=hidden_outputs;
				iNum=IH_SIZE_OUT;
				//weights=&weightHO;
				outputs=output_outputs;
				calcMatrixOutput(inputsTmp,outputs,weightHO,iNum,oNum);
			}
			//printOutputVector(outputs,oNum);

			iNum=outNum[currLayer];
			oNum=iNum;
			if(currLayer==IH_LAYER)
			{
				//weights=weightCH;
				inputsTmp=CH_outputs;
				calcMatrixOutput(inputsTmp,tmpOutputs,weightCH,iNum,oNum);
			}
			else
			{
				//weights=weightCO;
				inputsTmp=CO_outputs;
				calcMatrixOutput(inputsTmp,tmpOutputs,weightCO,iNum,oNum);
			}
			//printOutputVector(tmpOutputs,oNum);

			for(currOutput=0;currOutput<oNum;currOutput++)
			{
				outputs[currOutput]+=tmpOutputs[currOutput];
			}
			if(currLayer==HO_LAYER)
			{
				outputActivation(outputs, oNum,OUTERN);
			}
			else
			{
				outputActivation(outputs, oNum,INNERN);
			}
			//printOutputVector(outputs,oNum);
		}
		else
		{
			switch (currLayer)
			{
				case CH_LAYER:
					inputsTmp=hidden_outputs;
					outputs=CH_outputs;
					break;
				case CO_LAYER:
					inputsTmp=output_outputs;
					outputs=CO_outputs;
					break;
			}
			iNum=outNum[currLayer];
			oNum=iNum;
			if(currLayer==CH_LAYER)
			{
				//weights=weightHC;
				calcMatrixOutput(inputsTmp,outputs,weightHC,iNum,oNum);
			}
			else
			{
				//weights=weightOC;
				calcMatrixOutput(inputsTmp,outputs,weightOC,iNum,oNum);
			}
			//printOutputVector(outputs,oNum);

			outputActivation(outputs, oNum,INNERN);
			//printOutputVector(outputs,oNum);
		}
	}
}

void Reg_Neural(tOutputType *sensor_in)
{
	memcpy(input_outputs,sensor_in,INPUT_NEURONS*sizeof(tOutputType));
	sprintf(tmpS,"inputs copied\r\n");
	UARTSend(tmpS, strlen(tmpS));
	neuralBrainCycle();
	/*
	if (output_outputs[0]>0)
	{
		Mot_SetSpeed(RIGHT,FWD,abs(output_outputs[0]));
	}
	else if(output_outputs[0]<0)
	{
		Mot_SetSpeed(RIGHT,RWD,abs(output_outputs[0]));
	}
	else
	{
		Mot_SetSpeed(RIGHT,RWD,0);
	}
	if (output_outputs[1]>0)
	{
		Mot_SetSpeed(LEFT,FWD,abs(output_outputs[1]));
	}
	else if(output_outputs[1]<0)
	{
		Mot_SetSpeed(LEFT,RWD,abs(output_outputs[1]));
	}
	else
	{
		Mot_SetSpeed(LEFT,FWD,0);
	}
	*/

	if (output_outputs[0]>0)
	{
		Mot_SetSpeed(LEFT,FWD,abs(output_outputs[0]));
	}
	else if(output_outputs[0]<0)
	{
		Mot_SetSpeed(LEFT,RWD,abs(output_outputs[0]));
	}
	else
	{
		Mot_SetSpeed(LEFT,RWD,0);
	}
	if (output_outputs[1]>0)
	{
		Mot_SetSpeed(RIGHT,FWD,abs(output_outputs[1]));
	}
	else if(output_outputs[1]<0)
	{
		Mot_SetSpeed(RIGHT,RWD,abs(output_outputs[1]));
	}
	else
	{
		Mot_SetSpeed(RIGHT,FWD,0);
	}

	sprintf(tmpS,"left:%6d right:%6d \r\n",(int)output_outputs[0],(int)output_outputs[1]);
	UARTSend(tmpS, strlen(tmpS));
}

int32_t abs(int32_t num)
{
	if(num<0)
	{
		return num*(-1);
	}
	else
	{
		return num;
	}
}

void Reg_PrintWeightsAll()
{
	//tNeuronConn **weights=(tNeuronConn**)weightIH;
	sprintf(tmpS,"printing weightsIH\r\n");
	UARTSend(tmpS, strlen(tmpS));
	Reg_PrintWeights(weightIH,INPUT_NEURONS,HIDDEN_NEURONS);
	sprintf(tmpS,"printing weightsHO\r\n");
	UARTSend(tmpS, strlen(tmpS));
	Reg_PrintWeights(weightHO,HIDDEN_NEURONS,OUTPUT_NEURONS);
	sprintf(tmpS,"printing weightsCO\r\n");
	UARTSend(tmpS, strlen(tmpS));
	Reg_PrintWeights(weightCO,OUTPUT_NEURONS,OUTPUT_NEURONS);
	sprintf(tmpS,"printing weightsCH\r\n");
	UARTSend(tmpS, strlen(tmpS));
	Reg_PrintWeights(weightCH,HIDDEN_NEURONS,HIDDEN_NEURONS);

}

//void Reg_PrintWeights(tNeuronConn **weights,uint8_t iNum,uint8_t oNum)
//void Reg_PrintWeights(tNeuronConn *weights[],uint8_t iNum,uint8_t oNum)
void Reg_PrintWeights(tNeuronConn weights[][HIDDEN_NEURONS],uint8_t iNum,uint8_t oNum)
{
	uint8_t yAxis,xAxis;
	for(yAxis=0;yAxis<iNum;yAxis++)
	{
		for(xAxis=0;xAxis<oNum;xAxis++)
		{
			sprintf(tmpS,"%5d",weights[yAxis][xAxis]);
			UARTSend(tmpS, strlen(tmpS));
		}
		sprintf(tmpS,"\r\n");
		UARTSend(tmpS, strlen(tmpS));
	}
}


void printOutputVector(tOutputType *output,uint8_t oNum)
{
    uint8_t currOutput;
    printf("vector innards:");
    for(currOutput=0;currOutput<oNum;currOutput++)
    {
        sprintf(tmpS,"output%d %10d",currOutput,(int)output[currOutput]);
        UARTSend(tmpS, strlen(tmpS));
    }
    sprintf(tmpS,"\r\n");
    UARTSend(tmpS, strlen(tmpS));
}

#else
#endif
