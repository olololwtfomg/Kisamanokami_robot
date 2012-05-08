/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <olololwtfomg@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Jaroslav Mucka
 * ----------------------------------------------------------------------------
 */
#include "genetics.h"
#include "neural.h"
#include "fixedptc.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <omp.h>
#include "config.h"
#include <math.h>

uint32_t glInputNum,glOutputNum,glTestCaseNum;
uint32_t glPocetJedincov;
tTestCase *glTestCases;
tJedinec *glGenePool;
tNeuronConn **glTesting;

/*
private functions declatations
*/
void freeTestCases();
void destroyGenePool();
void Reg_PrintWeightsGen(tNeuronConn **weights,uint32_t iNum,uint32_t oNum);
void Reg_PrintWeightsAllGen(tJedinec *jedinec);
tNeuronConn **initConnMatrix(tNeuronConn **matrix,uint32_t iNum,uint32_t oNum);
void freeJedinecBrain(tJedinec *jedinec);
void initLayerOutputs(tJedinec *jedinec);
void destroyOutputs(tJedinec *jedinec);
void calcMatrixOutput(tOutputType *inputs,tOutputType *outputs,tNeuronConn **weights,uint32_t iNum, uint32_t oNum);
void printOutputVector(tOutputType *vector, uint32_t len);
void outputActivation(tOutputType *vector, uint32_t len,tNeuronType type);
tOutputType ActivateNeuron(tNeuronType type, tOutputType accum);
void jedinecBrainOutputs(tJedinec *jedinec,tInputType *inputs);
tErrorType getError(tOutputType* required, tOutputType* aquired);
void calculateErrors(tJedinec *jedinec);
void printJedinecErrors(tJedinec *jedinec);
void sortJedinci(tJedinec *jedinci);
void mutateWeights(tNeuronConn **weights,uint32_t iNum, uint32_t oNum);
void cloneAndMutateJedinec(tJedinec *clone,tJedinec* toClone);
void generationOrgy(tJedinec *jedinci);
void Reg_fPrintWeightsAllGen(tJedinec *jedinec);
void Reg_fPrintWeightsGen(tNeuronConn **weights,uint32_t iNum,uint32_t oNum);
void fPrintJedinecErrors(tJedinec *jedinec);
void testCaseCpy(tTestCase* dest, tTestCase *src);
void testCaseMakeNoise(tTestCase* testcase);
void printRealOutputs(tTestCase *testCases, tOutputType **realOutputs, uint32_t testCaseNum);
/*
functions definitions
*/
void initTestCases()
{
    uint32_t currTestCase,currElem,dummy;
    uint32_t inputNum,outputNum,testCaseNum;
    FILE *testCaseFile;
    testCaseFile=fopen(TESTCASE_FILE,"r");
    fscanf(testCaseFile,"%d%d%d",&inputNum,&outputNum,&testCaseNum);
    printf("inputNum=%d outputNum=%d testCaseNum=%d\r\n",inputNum,outputNum,testCaseNum);
    glInputNum=inputNum;
    glOutputNum=outputNum;
    //glTestCaseNum=testCaseNum*2;
    glTestCaseNum=testCaseNum;

    glTestCases=(tTestCase*)calloc(glTestCaseNum,sizeof(tTestCase));

    for(currTestCase=0;currTestCase<glTestCaseNum;currTestCase++)
    {
        glTestCases[currTestCase].inputs=(tInputType*)calloc(inputNum,sizeof(tInputType));
        glTestCases[currTestCase].outputs=(tOutputType*)calloc(outputNum,sizeof(tOutputType));
    }

    for(currTestCase=0;currTestCase<(glTestCaseNum);currTestCase++)
    {
        fscanf(testCaseFile,"%d",&dummy);
        printf("currTestCase=%d\r\n",currTestCase);
        printf("%5d",dummy);
        for(currElem=0;currElem<inputNum;currElem++)
        {
            fscanf(testCaseFile,"%d",&glTestCases[currTestCase].inputs[currElem]);
            printf("%7d",glTestCases[currTestCase].inputs[currElem]);
        }
        for(currElem=0;currElem<outputNum;currElem++)
        {
            fscanf(testCaseFile,"%d",&glTestCases[currTestCase].outputs[currElem]);
            printf("%7d",glTestCases[currTestCase].outputs[currElem]);
        }
        printf("\r\n");
    }
    fclose(testCaseFile);
    //make testcases with noise
    /*
    for(currTestCase=glTestCaseNum/2;currTestCase<(glTestCaseNum);currTestCase++)
    {
        testCaseCpy(&glTestCases[currTestCase],&glTestCases[currTestCase-testCaseNum/2]);
        testCaseMakeNoise(&glTestCases[currTestCase]);
    }
    */
    printf("Test cases Initialized \r\n");
}

void testCaseCpy(tTestCase* dest, tTestCase *src)
{
    memcpy(dest->inputs,src->inputs,sizeof(tInputType)*glInputNum);
    memcpy(dest->outputs,src->outputs,sizeof(tInputType)*glOutputNum);
}

void testCaseMakeNoise(tTestCase* testcase)
{
    uint32_t index;
    tInputType number=0;
    for(index=0;index<glInputNum;index++)
    {
        if(!(rand()%NOISE_PROB))
        {
            number=rand()%MAXFWD;
            if((rand()&(1<<(rand()%32))))
            {
                number*=(-1);
            }
        }
        if((number+testcase->inputs[index])>MAXFWD)
        {
            testcase->inputs[index]=MAXFWD;
        }
        if((number+testcase->inputs[index])<0)
        {
            testcase->inputs[index]=0;
        }
    }
}

void freeTestCases()
{
    uint32_t currTestCase;
    for(currTestCase=0;currTestCase<glTestCaseNum;currTestCase++)
    {
        free(glTestCases[currTestCase].inputs);
        free(glTestCases[currTestCase].outputs);
    }
    free(glTestCases);
    printf("Test Cases Freed\r\n");
}
void initJedinci(uint32_t num)
{
    uint32_t currJedinec;
    if(num<=1)
    {
        glPocetJedincov=0;
        printf("prilis malo jednincov");
        return;
    }
    else
    {
        glPocetJedincov=num;
        glGenePool=(tJedinec*)calloc(glPocetJedincov,sizeof(tJedinec));
        for(currJedinec=0;currJedinec<glPocetJedincov;currJedinec++)
        {
            glGenePool[currJedinec].UID=currJedinec;
            printf("initializing jedinec %d \r\n",currJedinec);
            Reg_InitNeuralGenetic(&glGenePool[currJedinec]);
            //Reg_PrintWeightsAllGen(&glGenePool[currJedinec]);
            initLayerOutputs(&glGenePool[currJedinec]);
        }
    }
    printf("vsetci jedinci inicializovany");
}

void Reg_InitNeuralGenetic(tJedinec* jedinec)
{
    jedinec->avgFinalError=0;
    jedinec->avgI2=0;
    jedinec->avgI3=0;
    //printf("initializing connections\r\n");

    jedinec->weightIH=initConnMatrix(jedinec->weightIH,IH_SIZE_IN,IH_SIZE_OUT);
    jedinec->weightHO=initConnMatrix(jedinec->weightHO,HO_SIZE_IN,HO_SIZE_OUT);
    jedinec->weightCH=initConnMatrix(jedinec->weightCH,CH_SIZE,CH_SIZE);
    jedinec->weightCO=initConnMatrix(jedinec->weightCO,CO_SIZE,CO_SIZE);
    jedinec->weightHC=initConnMatrix(jedinec->weightHC,CH_SIZE,CH_SIZE);
    jedinec->weightOC=initConnMatrix(jedinec->weightOC,CO_SIZE,CO_SIZE);
    //printf("printing connections\r\n");
    //Reg_PrintWeightsAllGen(jedinec);
	printf("Network for jedinec %d Initialized\r\n",jedinec->UID);
}

void destroyGenePool()
{
    uint32_t currJedinec;
    for(currJedinec=0;currJedinec<glPocetJedincov;currJedinec++)
    {
        freeJedinecBrain(&glGenePool[currJedinec]);
    }
    free(glGenePool);
    printf("Genes destroyed\r\n");
}

//kill and free everything
void holocaust()
{
    freeTestCases();
    destroyGenePool();
}

void Reg_PrintWeightsAllGen(tJedinec *jedinec)
{
    printf("printing all connections of jedinec %d\r\n",jedinec->UID);

	Reg_PrintWeightsGen(jedinec->weightIH,IH_SIZE_IN,IH_SIZE_OUT);
	Reg_PrintWeightsGen(jedinec->weightHO,HO_SIZE_IN,HO_SIZE_OUT);
	Reg_PrintWeightsGen(jedinec->weightCO,CO_SIZE,CO_SIZE);
	Reg_PrintWeightsGen(jedinec->weightCH,CH_SIZE,CH_SIZE);
	Reg_PrintWeightsGen(jedinec->weightOC,CO_SIZE,CO_SIZE);
	Reg_PrintWeightsGen(jedinec->weightHC,CH_SIZE,CH_SIZE);
}

void Reg_PrintWeightsGen(tNeuronConn **weights,uint32_t iNum,uint32_t oNum)
{
	uint32_t xAxis,yAxis;
	//printf("foo\r\n");
    for(yAxis=0;yAxis<iNum;yAxis++)
    {
        //printf("moo\r\n");
        for(xAxis=0;xAxis<oNum;xAxis++)
        {
            printf("%6d",weights[yAxis][xAxis]);
        }
        printf("\r\n");
    }
}

void Reg_fPrintWeightsAllGen(tJedinec *jedinec)
{
    printf("printing all connections of jedinec %d\r\n",jedinec->UID);

	Reg_fPrintWeightsGen(jedinec->weightIH,IH_SIZE_IN,IH_SIZE_OUT);
	Reg_fPrintWeightsGen(jedinec->weightHO,HO_SIZE_IN,HO_SIZE_OUT);
	Reg_fPrintWeightsGen(jedinec->weightCO,CO_SIZE,CO_SIZE);
	Reg_fPrintWeightsGen(jedinec->weightCH,CH_SIZE,CH_SIZE);
	Reg_fPrintWeightsGen(jedinec->weightOC,CO_SIZE,CO_SIZE);
	Reg_fPrintWeightsGen(jedinec->weightHC,CH_SIZE,CH_SIZE);
}

void Reg_fPrintWeightsGen(tNeuronConn **weights,uint32_t iNum,uint32_t oNum)
{
    FILE *outputFile;
    uint32_t xAxis,yAxis;
    outputFile=fopen(OUTPUT_FILE,"a+");
    fprintf(outputFile,"{\r\n");
    for(yAxis=0;yAxis<iNum;yAxis++)
    {
        //printf("moo\r\n");
        fprintf(outputFile,"{");
        for(xAxis=0;xAxis<oNum;xAxis++)
        {
            fprintf(outputFile,"%6d,",weights[yAxis][xAxis]);
        }
        fprintf(outputFile,"},\r\n");
    }
    fprintf(outputFile,"};\r\n");
    fclose(outputFile);
}


tNeuronConn **initConnMatrix(tNeuronConn **matrix,uint32_t iNum,uint32_t oNum)
{
    uint32_t xAxis,yAxis;
    matrix=(tNeuronConn**)calloc(iNum,sizeof(tNeuronConn*));
    for(yAxis=0;yAxis<iNum;yAxis++)
    {
        matrix[yAxis]=(tNeuronConn*)calloc(oNum,sizeof(tNeuronConn));
        for(xAxis=0;xAxis<oNum;xAxis++)
        {
            if(rand()%2)
            {
                matrix[yAxis][xAxis]=rand()%MAXWEIGHT;
            }
            else
            {
                matrix[yAxis][xAxis]=(rand()%MAXWEIGHT)*(-1);
            }
            //printf("%6d",matrix[yAxis][xAxis]);
        }
        //printf("\r\n");
    }
    return matrix;
}

//or should i say kill >:D
void freeJedinecBrain(tJedinec *jedinec)
{
    uint32_t yAxis;
    for(yAxis=0;yAxis<IH_SIZE_IN;yAxis++)
    {
        free(jedinec->weightIH[yAxis]);
    }
    for(yAxis=0;yAxis<HO_SIZE_IN;yAxis++)
    {
        free(jedinec->weightHO[yAxis]);
    }
    for(yAxis=0;yAxis<CH_SIZE;yAxis++)
    {
        free(jedinec->weightCH[yAxis]);
    }
    for(yAxis=0;yAxis<CO_SIZE;yAxis++)
    {
        free(jedinec->weightCO[yAxis]);
    }
    for(yAxis=0;yAxis<CO_SIZE;yAxis++)
    {
        free(jedinec->weightHC[yAxis]);
    }
    for(yAxis=0;yAxis<CO_SIZE;yAxis++)
    {
        free(jedinec->weightOC[yAxis]);
    }
    free(jedinec->weightIH);
    free(jedinec->weightHO);
    free(jedinec->weightCH);
    free(jedinec->weightCO);
    free(jedinec->weightHC);
    free(jedinec->weightOC);
    destroyOutputs(jedinec);
}

void initLayerOutputs(tJedinec *jedinec)
{
    uint32_t currLayer;
    uint32_t outNum;
    jedinec->layerOutputs=(tOutputType**)calloc(CO_LAYER+1,sizeof(tOutputType*));
    for(currLayer=IH_LAYER;currLayer<(CO_LAYER+1);currLayer++)
    {
        switch(currLayer)
        {
            case IH_LAYER: outNum=IH_SIZE_OUT; break;
            case HO_LAYER: outNum=HO_SIZE_OUT; break;
            case CH_LAYER: outNum=CH_SIZE; break;
            case CO_LAYER: outNum=CO_SIZE; break;
        }
        jedinec->layerOutputs[currLayer]=(tOutputType*)calloc(outNum,sizeof(tOutputType));
    }
    jedinec->realOutputs=(tOutputType**)calloc(glTestCaseNum,sizeof(tOutputType*));
    for(currLayer=0;currLayer<glTestCaseNum;currLayer++)
    {
        jedinec->realOutputs[currLayer]=(tOutputType*)calloc(HO_SIZE_OUT,sizeof(tOutputType));
    }
}

void destroyOutputs(tJedinec *jedinec)
{
    uint32_t currLayer;
    for(currLayer=IH_LAYER;currLayer<CO_LAYER;currLayer++)
    {
        free(jedinec->layerOutputs[currLayer]);
    }
    free(jedinec->layerOutputs);
    for(currLayer=0;currLayer<glTestCaseNum;currLayer++)
    {
        free(jedinec->realOutputs[currLayer]);
    }
    free(jedinec->realOutputs);
}

void calcMatrixOutput(tOutputType *inputs,tOutputType *outputs,tNeuronConn **weights,uint32_t iNum, uint32_t oNum)
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



void printOutputVector(tOutputType *vector, uint32_t len)
{
    uint32_t currOutput;
    printf("vector innards:");
    for(currOutput=0;currOutput<len;currOutput++)
    {
        printf("%10d",vector[currOutput]);
    }
    printf("\r\n");
}

void outputActivation(tOutputType *vector, uint32_t len,tNeuronType type)
{
    uint32_t currOutput;
    for(currOutput=0;currOutput<len;currOutput++)
    {
        vector[currOutput]=Reg_ActivateNeuron(type,vector[currOutput]);
    }
}

tOutputType ActivateNeuron(tNeuronType type, tOutputType accum)
{
    //char frc[12];
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
        //printf("accum=%d\r\n",accum);
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
		//fixedpt_str(tmp,frc);
		//printf("tmp=%s\r\n",frc);
		tmp=fixedpt_div(fpAccum,tmp);
		tmp=fixedpt_mul(tmp,fpMaxfwd);
		break;
	}
	//printf("lol");
	return fixedpt_toint(tmp);
}

void jedinecBrainOutputs(tJedinec *jedinec,tInputType *inputs)
{
    uint8_t currLayer;
    uint32_t outNum[]={IH_SIZE_OUT,HO_SIZE_OUT,CH_SIZE,CO_SIZE};
    tOutputType *inputsTmp;
    tOutputType *outputs;
    tNeuronConn **weights;
    uint32_t iNum;
    uint32_t oNum;
    uint32_t currOutput;
    tOutputType *tmpOutputs;
    for(currLayer=IH_LAYER;currLayer<(CO_LAYER+1);currLayer++)
    {
        if(currLayer<CH_LAYER)
        {
            if(currLayer==IH_LAYER)
            {
                inputsTmp=inputs;
                iNum=glInputNum;
                weights=jedinec->weightIH;
            }
            else
            {
                inputsTmp=jedinec->layerOutputs[IH_LAYER];
                iNum=IH_SIZE_OUT;
                weights=jedinec->weightHO;
            }
            oNum=outNum[currLayer];
            outputs=jedinec->layerOutputs[currLayer];
            calcMatrixOutput(inputsTmp,outputs,weights,iNum,oNum);
            if(currLayer==IH_LAYER)
            {
                weights=jedinec->weightCH;
            }
            else
            {
                weights=jedinec->weightCO;
            }
            inputsTmp=jedinec->layerOutputs[currLayer+CH_LAYER];
            iNum=outNum[currLayer];
            oNum=iNum;
            tmpOutputs=(tOutputType*)calloc(outNum[currLayer],sizeof(tOutputType));
            calcMatrixOutput(inputsTmp,tmpOutputs,weights,iNum,oNum);
            for(currOutput=0;currOutput<oNum;currOutput++)
            {
                outputs[currOutput]+=tmpOutputs[currOutput];
            }
            free(tmpOutputs);
            if(currLayer==HO_LAYER)
            {
                outputActivation(outputs, oNum,OUTERN);
            }
            else
            {
                outputActivation(outputs, oNum,INNERN);
            }
        }
        else
        {
            inputsTmp=jedinec->layerOutputs[currLayer-CH_LAYER];
            outputs=jedinec->layerOutputs[currLayer];
            iNum=outNum[currLayer];
            oNum=iNum;
            if(currLayer==CH_LAYER)
            {
                weights=jedinec->weightHC;
            }
            else
            {
                weights=jedinec->weightOC;
            }
            calcMatrixOutput(inputsTmp,outputs,weights,iNum,oNum);
            outputActivation(outputs, oNum,INNERN);
        }
    }
}

void calculateErrors(tJedinec *jedinec)
{
    uint32_t currTestCase;
    uint32_t currTest;
    tErrorType tmpError;
    for(currTestCase=0;currTestCase<glTestCaseNum;currTestCase++)
    {
        for(currTest=0;currTest<TESTS_TOTAL;currTest++)
        {
            jedinecBrainOutputs(jedinec,glTestCases[currTestCase].inputs);
            tmpError=getError(glTestCases[currTestCase].outputs,jedinec->layerOutputs[HO_LAYER]);
            jedinec->avgI2+=tmpError;
            jedinec->avgI3+=tmpError*tmpError;
        }
       // printf("tmpError %" PRIu64 "\n", tmpError);
        //printf("%llu\r\n",tmpError);
        jedinec->avgFinalError+=tmpError;
        memcpy(jedinec->realOutputs[currTestCase],jedinec->layerOutputs[HO_LAYER],HO_SIZE_OUT*sizeof(tOutputType));
        //printJedinecErrors(jedinec);
    }
    jedinec->avgFinalError/=glTestCaseNum;
    jedinec->avgI2/=glTestCaseNum;
    jedinec->avgI3/=glTestCaseNum;
}

void printRealOutputs(tTestCase *testCases, tOutputType **realOutputs, uint32_t testCaseNum)
{
    uint32_t currCase;
    uint32_t itemNum;
    printf("testcases:%d inputs:%d outputs:%d\r\n",testCaseNum,glInputNum,glOutputNum);
    //getchar();
    for(currCase=0;currCase<testCaseNum;currCase++)
    {
        //getchar();
        //test case inputs
        for(itemNum=0;itemNum<glInputNum;itemNum++)
        {
            //getchar();
            printf("%6d",testCases[currCase].inputs[itemNum]);
        }
        for(itemNum=0;itemNum<glOutputNum;itemNum++)
        {
            printf("%6d",realOutputs[currCase][itemNum]);
        }
        printf("\r\n");
        //getchar();
    }
}

tErrorType getError(tOutputType* required, tOutputType* aquired)
{
    tErrorType tmpError=0;
    uint32_t currOutput;
    for(currOutput=0;currOutput<glOutputNum;currOutput++)
    {
        tmpError+=abs(required[currOutput]-aquired[currOutput]);
    }
    return tmpError;
}

void printJedinecErrors(tJedinec *jedinec)
{
    double errorLast,I2,I3;
    //by ma sakramensky zaujmalo preco to nezozere %llu
    printf("jedinec UID %d finalERR=%" PRIu64 " I2=%" PRIu64 " I3=%" PRIu64 "\r\n",jedinec->UID,jedinec->avgFinalError,jedinec->avgI2,jedinec->avgI3);
    errorLast=(double)jedinec->avgFinalError/((double)MAXFWD*2.0);
    I2=(double)jedinec->avgI2/((double)MAXFWD*(double)TESTS_TOTAL*2.0);
    I3=sqrt((double)jedinec->avgI3/(double)TESTS_TOTAL)/((double)MAXFWD*2.0);
    printf("jedinec UID %d finalERR=%f I2=%f I3=%f \r\n",jedinec->UID,errorLast,I2,I3);
}

void fPrintJedinecErrors(tJedinec *jedinec)
{
    FILE *outputFile;
    double errorLast,I2,I3;
    outputFile=fopen(OUTPUT_FILE,"a+");
    //by ma sakramensky zaujmalo preco to nezozere %llu
    fprintf(outputFile,"jedinec UID %d finalERR=%" PRIu64 " I2=%" PRIu64 " I3=%" PRIu64 "\r\n",jedinec->UID,jedinec->avgFinalError,jedinec->avgI2,jedinec->avgI3);
    errorLast=(double)jedinec->avgFinalError/((double)MAXFWD*2.0);
    I2=(double)jedinec->avgI2/((double)MAXFWD*(double)TESTS_TOTAL*2.0);
    I3=sqrt((double)jedinec->avgI3/(double)TESTS_TOTAL)/((double)MAXFWD*2.0);
    fprintf(outputFile,"jedinec UID %d finalERR=%f I2=%f I3=%f \r\n",jedinec->UID,errorLast,I2,I3);
    fclose(outputFile);
}

void generationLife(tJedinec *jedinci)
{
    uint32_t currJedinec;
    omp_set_num_threads(MAX_THREADS);
#if USE_THREADS ==1
    #pragma omp parallel private(currJedinec)
    {
        #pragma omp for schedule(dynamic,CHUNK_SIZE)
#endif
    for(currJedinec=0;currJedinec<JEDINEC_NUM;currJedinec++)
    {
        calculateErrors(&jedinci[currJedinec]);
        //printJedinecErrors(&jedinci[currJedinec]);
        //printf("weight pointer:%d\r\n",(uint32_t)jedinci[currJedinec].weightCH);
        //Reg_PrintWeightsAllGen(&jedinci[currJedinec]);
    }
#if USE_THREADS ==1
    }
#endif
    sortJedinci(jedinci);
    /*
    for(currJedinec=0;currJedinec<JEDINEC_NUM;currJedinec++)
    {
        printJedinecErrors(&jedinci[currJedinec]);
        //printf("weight pointer:%d\r\n",(uint32_t)jedinci[currJedinec].weightCH);
        //Reg_PrintWeightsAllGen(&jedinci[currJedinec]);
    }
    */
}

void sortJedinci(tJedinec *jedinci)
{
    int compJedinec(const void *first,const void *second);
    qsort(jedinci, JEDINEC_NUM, sizeof(tJedinec), compJedinec);
}

int compJedinec(const void *first,const void *second)
{
    tJedinec *a=(tJedinec *)first;
    tJedinec *b=(tJedinec *)second;
    int returnNum=0;
    if((a->avgFinalError)==(b->avgFinalError))
    {
        if((a->avgI2)==(b->avgI2))
        {
            returnNum=(a->avgI3)-(b->avgI3);
        }
        else
        {
            returnNum=(a->avgI2)-(b->avgI2);
        }
    }
    else
    {
        returnNum=(a->avgFinalError)-(b->avgFinalError);
    }
    return returnNum;
}

void cloneAndMutateJedinec(tJedinec *clone,tJedinec* toClone)
{
    void weightCpy(tNeuronConn **dest,tNeuronConn **src,uint32_t iNum, uint32_t oNum);
    //printf("cloning\r\n");
    weightCpy(clone->weightIH,toClone->weightIH,IH_SIZE_IN,IH_SIZE_OUT);
    //printf("cloning\r\n");
    weightCpy(clone->weightHO,toClone->weightHO,HO_SIZE_IN,HO_SIZE_OUT);
    //printf("cloning\r\n");
    weightCpy(clone->weightCO,toClone->weightCO,CO_SIZE,CO_SIZE);
    //printf("cloning\r\n");
    weightCpy(clone->weightCH,toClone->weightCH,CH_SIZE,CH_SIZE);
    //printf("cloning\r\n");
    weightCpy(clone->weightOC,toClone->weightOC,CO_SIZE,CO_SIZE);
    //printf("cloning\r\n");
    weightCpy(clone->weightHC,toClone->weightHC,CH_SIZE,CH_SIZE);
    /*
    memcpy(clone->weightIH,toClone->weightIH,sizeof(tNeuronConn)*IH_SIZE_IN*IH_SIZE_OUT);
	memcpy(clone->weightIH,toClone->weightHO,sizeof(tNeuronConn)*HO_SIZE_IN*HO_SIZE_OUT);
	memcpy(clone->weightIH,toClone->weightCO,sizeof(tNeuronConn)*CO_SIZE*CO_SIZE);
	memcpy(clone->weightIH,toClone->weightCH,sizeof(tNeuronConn)*CH_SIZE*CH_SIZE);
	memcpy(clone->weightIH,toClone->weightOC,sizeof(tNeuronConn)*CO_SIZE*CO_SIZE);
	memcpy(clone->weightIH,toClone->weightHC,sizeof(tNeuronConn)*CH_SIZE*CH_SIZE);
	*/
	//printf("mutating\r\n");
	mutateWeights(clone->weightIH,IH_SIZE_IN,IH_SIZE_OUT);
	mutateWeights(clone->weightHO,HO_SIZE_IN,HO_SIZE_OUT);
	mutateWeights(clone->weightCO,CO_SIZE,CO_SIZE);
	mutateWeights(clone->weightCH,CH_SIZE,CH_SIZE);
	mutateWeights(clone->weightOC,CO_SIZE,CO_SIZE);
	mutateWeights(clone->weightHC,CH_SIZE,CH_SIZE);
    //printf("new clone ready\r\n");
}

void weightCpy(tNeuronConn **dest,tNeuronConn **src,uint32_t iNum, uint32_t oNum)
{
    uint32_t yAxis;
    for(yAxis=0;yAxis<iNum;yAxis++)
    {
        memcpy(dest[yAxis],src[yAxis],sizeof(tNeuronConn)*oNum);
    }
}

void mutateWeights(tNeuronConn **weights,uint32_t iNum, uint32_t oNum)
{
    uint32_t xAxis,yAxis;
    tNeuronConn number=0;
    uint8_t mutationProb=(rand()%MUTATION_PROB)+1;
    for(yAxis=0;yAxis<iNum;yAxis++)
    {
        for(xAxis=0;xAxis<oNum;xAxis++)
        {

            if(!(rand()%mutationProb))
            {
                if((rand()&(1<<(rand()%32))))
                {
                    number=rand()%MAXWEIGHT;
                }
                else
                {
                    number=(rand()%MAXWEIGHT)*-1;
                }
                weights[yAxis][xAxis]+=number;
            }
        }
    }
}

//or should i say making a pure race
void generationOrgy(tJedinec *jedinci)
{
    //clone the best and make mutants of the rest
    uint32_t currJedinec;
    //the first should be the best ... but who knows
    for(currJedinec=1;currJedinec<JEDINEC_NUM;currJedinec++)
    {
        //printf("currJedinec %d\r\n",currJedinec);
        cloneAndMutateJedinec(&glGenePool[currJedinec],&glGenePool[0]);
        //Reg_PrintWeightsAllGen(&glGenePool[currJedinec]);
    }
    printf("a new power is rising\r\n");
}

void generationResetErrors(tJedinec *jedinci)
{
    uint32_t currJedinec;
    for(currJedinec=0;currJedinec<JEDINEC_NUM;currJedinec++)
    {
        jedinci[currJedinec].avgFinalError=0;
        jedinci[currJedinec].avgI2=0;
        jedinci[currJedinec].avgI3=0;
    }
}

void generationFermentation()
{
    void generationResetErrors(tJedinec *jedinci);
    tErrorType lastError=0;
    tErrorType lastI2=0;
    tErrorType lastI3=0;
    uint8_t repeat=0;
    while (repeat<MAX_REPEAT)
    {
        generationLife(glGenePool);
        generationOrgy(glGenePool);
        if((glGenePool[0].avgFinalError==lastError)&&(glGenePool[0].avgI2==lastI2)&&(glGenePool[0].avgI3==lastI3))
        {
            repeat++;
        }
        else
        {
            lastError=glGenePool[0].avgFinalError;
            lastI2=glGenePool[0].avgI2;
            lastI3=glGenePool[0].avgI3;
            repeat=0;
        }
        printJedinecErrors(&glGenePool[0]);
        //

        //
        //getchar();
        generationResetErrors(glGenePool);
        if(!(rand()%KONTROLA))
        {
            fPrintJedinecErrors(&glGenePool[0]);
            Reg_fPrintWeightsAllGen(&glGenePool[0]);
            printRealOutputs(glTestCases, glGenePool[0].realOutputs, glTestCaseNum);
        }
    }
    Reg_fPrintWeightsAllGen(&glGenePool[0]);
    printRealOutputs(glTestCases, glGenePool[0].realOutputs, glTestCaseNum);
}

void testFunc()
{

    /*
    //test 0
    tOutputType *inputs=glTestCases[0].inputs;
    tOutputType *outputs=glGenePool[0].layerOutputs[IH_LAYER];
    tNeuronConn **weights=glGenePool[0].weightIH;
    uint32_t iNum=IH_SIZE_IN;
    uint32_t oNum=IH_SIZE_OUT;
    calcMatrixOutput(inputs,outputs,weights,iNum,oNum);
    printOutputVector(outputs,IH_SIZE_OUT);
    outputActivation(outputs, IH_SIZE_OUT,OUTERN);
    printOutputVector(outputs,IH_SIZE_OUT);
    */
    //jedinecBrainOutputs(&glGenePool[0],glTestCases[0].inputs);
    /*
    uint8_t currLayer;
    uint32_t outNum[]={IH_SIZE_OUT,HO_SIZE_OUT,CH_SIZE,CO_SIZE};
    tOutputType *outputs;
    for(currLayer=IH_LAYER;currLayer<(CO_LAYER+1);currLayer++)
    {
        printf("%d. outputs\r\n",currLayer);
        outputs=glGenePool[0].layerOutputs[currLayer];
        printOutputVector(outputs,outNum[currLayer]);
    }
    */
    /*
    generationLife(glGenePool);
    Reg_PrintWeightsAllGen(&glGenePool[0]);
    Reg_PrintWeightsAllGen(&glGenePool[1]);
    cloneAndMutateJedinec(&glGenePool[1],&glGenePool[0]);
    printf("jedinci zmutovany\r\n");
    Reg_PrintWeightsAllGen(&glGenePool[0]);
    Reg_PrintWeightsAllGen(&glGenePool[1]);
    */
    generationLife(glGenePool);
    generationOrgy(glGenePool);
}
