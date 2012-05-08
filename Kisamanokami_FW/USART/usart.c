/* Private functions ---------------------------------------------------------*/

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "usart.h"
#define NUM 10

int j;
volatile int i=0;
char name[NUM+1] = {'\0'};

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void UARTSend(const char *pucBuffer, unsigned long ulCount);

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles USARTx global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
    if ((USED_USART->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
			i = USART_ReceiveData(USED_USART);
			USART1->SR &= ~USART_FLAG_RXNE;
	}
}

void USART_Reply()
{
	char c=(char)(i&0xff);
	asm volatile("CPSID I");
	if(i!=0)
	{
		UARTSend(&c, 1);
		i=0;
	}
	asm volatile("CPSIE I");
}

/*******************************************************************************
* Function Name  : usart_rxtx
* Description    : Print "Welcome to CooCox!"  on Hyperterminal via USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void usart_rxtx(void)
{

    /* Enable USART1 and GPIOA clock */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* NVIC Configuration */
    NVIC_Configuration();


    /* Configure the GPIOs */
  //  GPIO_Configuration();

    /* Configure the USART1 */
    USART_Configuration();

    /* Enable the USART1 Receive interrupt: this interrupt is generated when the
         USART1 receive data register is not empty */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    /* print welcome information */
//    UARTSend(menu, sizeof(menu));

//    while(1)
//	{
//		if(name[k] != '\0')
//		{
//			UARTSend((const unsigned char*)&name[k++],1);
//		}
//		if(k > NUM)
//		k = 0;
//	}
}

void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate =38400;//9600;//57600; //115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART3, &USART_InitStructure);

  /* Enable USART1 */
  USART_Cmd(USART3, ENABLE);
}
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : UARTSend
* Description    : Send a string to the UART.
* Input          : - pucBuffer: buffers to be printed.
*                : - ulCount  : buffer's length
* Output         : None
* Return         : None
*******************************************************************************/
void UARTSend(const char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
    	/* Loop until the end of transmission */
    	while(USART_GetFlagStatus(USED_USART, USART_FLAG_TC) == RESET);
        USART_SendData(USED_USART, (uint16_t) *pucBuffer++);
    }
}
