/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <olololwtfomg@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Jaroslav Mucka
 * ----------------------------------------------------------------------------
 */


/*
Why do most functions begin with reg... well after some simulations in matlab,
the outputs were very similar to those of a PSD regulator. So i decided why not
use them as a regulator for a line follower robot servo mechanism.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "fixedptc.h"
#include "neural.h"
#include "genetics.h"

#include <omp.h>

#if USE_THREADS==1




#endif



int Reg_ActivateNeuronBck(unsigned char type, int accum);
void Reg_PrintWeights(int *weights,unsigned char iNum,unsigned char oNum);

static int weightIH[INPUT_NEURONS*HIDDEN_NEURONS];
static int weightHO[HIDDEN_NEURONS*OUTPUT_NEURONS];
static int weightCH[HIDDEN_NEURONS*HIDDEN_NEURONS];
static int weightCO[OUTPUT_NEURONS*OUTPUT_NEURONS];

static int input_outputs[INPUT_NEURONS];
static int hidden_outputs[HIDDEN_NEURONS];
static int output_outputs[OUTPUT_NEURONS];
static int CH_outputs[HIDDEN_NEURONS];
static int CO_outputs[OUTPUT_NEURONS];

static int hidden_bck[HIDDEN_NEURONS*STEPS];
static int output_bck[OUTPUT_NEURONS*STEPS];
static int CH_bck[HIDDEN_NEURONS*STEPS];
static int CO_bck[OUTPUT_NEURONS*STEPS];

int rand1(int lim);
int abs(int num);
void srand1();

static long a ;

int rand1(int lim)
{
	//a = (a * 125) % 2796203; modulo konstanta je na pizzu
	a=rand();
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

void srand1()
{
    srand(time(NULL));
    a=rand();
}

int abs(int num)
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

void Reg_InitNeural()
{
	unsigned char i;
	srand1();
	for (i=0;i<INPUT_NEURONS*HIDDEN_NEURONS;i++)
	{
		weightIH[i]=rand1(MAXWEIGHT);
	}
	for (i=0;i<HIDDEN_NEURONS*OUTPUT_NEURONS;i++)
	{
		weightHO[i]=rand1(MAXWEIGHT);
	}
	for (i=0;i<HIDDEN_NEURONS*HIDDEN_NEURONS;i++)
	{
		weightCH[i]=rand1(MAXWEIGHT);
	}
	for (i=0;i<OUTPUT_NEURONS*OUTPUT_NEURONS;i++)
	{
		weightCO[i]=rand1(MAXWEIGHT);
	}
    for(i=0;i<HIDDEN_NEURONS;i++)
    {
        CH_outputs[i]=0;
    }
    for(i=0;i<OUTPUT_NEURONS;i++)
    {
        CO_outputs[i]=0;
    }

	printf("Network Initialized\r\n");
}

void Reg_Backpropagate()
{

}

void Reg_LayerOutputs(  unsigned char type, unsigned char step)
{
	unsigned char iNum=0;
	unsigned char oNum=0;
	int *output;
	int *input;
	int *accum=NULL;
	unsigned char i,j;
	int *weights=NULL;
	switch (type)
	{
	case CONN_IH:
		input=input_outputs;
		output=hidden_outputs;
		weights=weightIH;
		iNum=INPUT_NEURONS;
		oNum=HIDDEN_NEURONS;
#if DEBUG==1
        printf("calculating IH\r\n");
#endif
		break;
	case CONN_HO:
		input=hidden_outputs;
		output=output_outputs;
		weights=weightHO;
		iNum=HIDDEN_NEURONS;
		oNum=OUTPUT_NEURONS;
#if DEBUG==1
        printf("calculating HO\r\n");
#endif
		break;
	case CONN_OC:
		input=output_outputs;
		output=CO_outputs;
		iNum=OUTPUT_NEURONS;
		oNum=OUTPUT_NEURONS;
		accum=CO_bck;
#if DEBUG==1
        printf("calculating OC\r\n");
#endif
		break;
	case CONN_HC:
		input=hidden_outputs;
		output=CH_outputs;
		iNum=HIDDEN_NEURONS;
		oNum=HIDDEN_NEURONS;
		accum=CH_bck;
#if DEBUG==1
        printf("calculating HC\r\n");
#endif
		break;
	}
#if USE_THREADS ==1
	omp_set_num_threads(MAX_THREADS);
#endif
	if((type==CONN_HO)||(type==CONN_IH))
	{
	    // first calculate accumulate input to neuron
#if USE_THREADS ==1
	    #pragma omp parallel private(i)
	    {
	        #pragma omp for schedule(dynamic,CHUNK_SIZE)
#endif
           // printf("inum=%d onum=%d\r\n",iNum,oNum);
            for (i=0;i<oNum;i++)
            {
                output[i]=0;
                for(j=0;j<iNum;j++)
                {
                  //  printf("i:%3d,j:%3d,j+i*oNum:%4d\r\n",i,j,j+i*oNum);
                    output[i]+=input[j]*weights[j+i*oNum];
                }
                #if DEBUG_CONN==1
                printf("output %d before context =%d ",i,output[i]);
                #endif
            }
            #if DEBUG_CONN==1
                printf("\r\n");
            #endif
#if USE_THREADS ==1
	    }
#endif
       // getchar();
		if(type==CONN_HO)
		{
			input=CO_outputs;
			output=output_outputs;
			weights=weightCO;
			iNum=OUTPUT_NEURONS;
			oNum=OUTPUT_NEURONS;
			accum=output_bck;
		}
		else
		{
			input=CH_outputs;
			output=hidden_outputs;
			weights=weightCH;
			iNum=HIDDEN_NEURONS;
			oNum=HIDDEN_NEURONS;
			accum=hidden_bck;
		}
		// then calculate from context
#if USE_THREADS ==1
		#pragma omp parallel private(i)
	    {
	        #pragma omp for schedule(dynamic,CHUNK_SIZE)
#endif
           // printf("inum=%d onum=%d\r\n",iNum,oNum);
            for (i=0;i<oNum;i++)
            {
                for(j=0;j<iNum;j++)
                {
                   // printf("i:%3d,j:%3d,j+i*oNum:%4d\r\n",i,j,j+i*oNum);
                    output[i]+=input[j]*weights[j+i*oNum];
                }
                #if DEBUG_CONN==1
                printf("output %d after context =%d ",i,output[i]);
                #endif
                //printf("output%d ba:%8d\r\n",i,output[i]);
               // printf("step: %d\r\n",step);
                accum[i+step*STEPS]=output[i];
                //printf("foo\r\n");
                if(type==CONN_HO)
                {
                    //printf("i=%i\r\n",i);
                    output[i]=Reg_ActivateNeuron(OUTER, output[i]);
                    //printf("fubar\r\n");
                }
                else
                {
                    output[i]=Reg_ActivateNeuron(INNER, output[i]);
                    //printf("KILL\r\n");
                }
                 //printf("moo\r\n");
                //printf("output%d aa:%8d\r\n",i,output[i]);
            }
            #if DEBUG_CONN==1
                printf("\r\n");
            #endif
#if USE_THREADS ==1
	    }
#endif
    //getchar();
	}
	else
	{
#if USE_THREADS ==1
	    #pragma omp parallel private(i)
	    {
	        #pragma omp for schedule(dynamic,CHUNK_SIZE)
#endif
            for (i=0;i<oNum;i++)
            {
                output[i]=input[i];
                /*
                output[i]=0;
                for(j=0;j<iNum;j++)
                {
                    output[i]+=input[j];
                }
                #if DEBUG_CONN==1
                printf("output %d of context =%d ",i,output[i]);
                #endif
                //printf("output%d ba:%8d\r\n",i,output[i]);
                accum[i+step*STEPS]=output[i];
                output[i]=Reg_ActivateNeuron(INNER, output[i]);
                //printf("output%d aa:%8d\r\n",i,output[i]);
                */
            }
            #if DEBUG_CONN==1
                printf("\r\n");
            #endif
#if USE_THREADS ==1
	    }
#endif
	}
#if DEBUG==1

        for(i=0;i<oNum;i++)
        {
            printf("%d:%6d ",i,output[i]);
        }
        printf("done\r\n");
#endif
}

int Reg_ActivateNeuron(unsigned char type, int accum)
{
    //char frc[12];
    fixedpt fpAccum,fpMaxfwd,tmp;
    tmp=fixedpt_fromint(0);
    if(type==INNER)
    {
        fpMaxfwd=fixedpt_fromint(MAX_NEURON_OUT);
    }
    else
    {
        fpMaxfwd=fixedpt_fromint(MAXFWD);
    }
    if((accum<FIXED_POS_LIMIT)&&(accum>FIXED_NEG_LIMIT))
    {
        //printf("accum=%d\r\n",accum);
        fpAccum=fixedpt_fromint(accum);
    }
    else
    {
        if(accum>FIXED_POS_LIMIT)
        {
            if(type==INNER)
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
            if(type==INNER)
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
	case INNER:
        tmp=fixedpt_abs(fpAccum);
		tmp=fixedpt_add(tmp,FIXEDPT_ONE);
		tmp=fixedpt_div(fpAccum,tmp);
		tmp=fixedpt_mul(tmp,fpMaxfwd);
		break;
	case OUTER:
        tmp=fixedpt_abs(fpAccum);
		tmp=fixedpt_add(tmp,fpMaxfwd);
		//fixedpt_str(tmp,frc);
		//printf("tmp=%s\r\n",frc);
		tmp=fixedpt_div(fpAccum,tmp);
		tmp=fixedpt_mul(tmp,fpMaxfwd);
		break;
	}
	//printf("lol");
	return fixedpt_toint(tmp);
}

int Reg_ActivateNeuronBck(unsigned char type, int accum)
{
    int output=0;
	switch (type)
	{
	case INNER:
		output= (1/(1+abs(accum))*(1+abs(accum)));
		break;
	case OUTER:
		output= (MAXFWD/(MAXFWD+abs(accum))*(MAXFWD+abs(accum)))*MAXFWD;
		break;
	}
	return output;
}

void Reg_Neural(unsigned short *sensor_in, unsigned char steps)
{
    unsigned char i;
	memcpy(input_outputs,sensor_in,INPUT_NEURONS*sizeof(unsigned short));
	for(i=0;i<steps;i++)
    {
#if DEBUG==1
        printf("calculating output: %d \r\n",i);
#endif
        Reg_LayerOutputs(CONN_IH,i);
        Reg_LayerOutputs(CONN_HO,i);
        Reg_LayerOutputs(CONN_HC,i);
        Reg_LayerOutputs(CONN_OC,i);
        Reg_PrintOutputs();
#if DEBUG==1
        printf("output %d calculated\r\n",i);
#endif
    }
}

void Reg_PrintWeightsAll()
{
	Reg_PrintWeights(weightIH,INPUT_NEURONS,HIDDEN_NEURONS);
	Reg_PrintWeights(weightHO,HIDDEN_NEURONS,OUTPUT_NEURONS);
	Reg_PrintWeights(weightCO,OUTPUT_NEURONS,OUTPUT_NEURONS);
	Reg_PrintWeights(weightCH,HIDDEN_NEURONS,HIDDEN_NEURONS);
    #if DEBUG==1
        printf("\r\n");
    #endif
}

//can be used to print pseudo two dimensional arrays
void Reg_PrintWeights(int *weights,unsigned char iNum,unsigned char oNum)
{
	unsigned char i,j;


	for(i=0;i<oNum;i++)
	{
		for(j=0;j<iNum;j++)
		{
			printf("%5d",weights[j+i*oNum]);
		}
		printf("\r\n");

	}

}

void Reg_PrintOutputs()
{
    printf("Outputs 1:%6d 2:%6d\r\n",output_outputs[0],output_outputs[1]);
}


