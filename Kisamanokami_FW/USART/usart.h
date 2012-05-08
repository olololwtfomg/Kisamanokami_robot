#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#define USED_USART USART3

void USART_Reply();

void usart_rxtx(void);
void UARTSend(const char *pucBuffer, unsigned long ulCount);


#endif
