#include <DrvUART.h>

void PrintChar(char c)
{
	/* Send a char like: 
	   while(Transfer not completed);
	   Transmit a char;
	*/	
	uint8_t ch;
	ch = c;
	DrvUART_Write(UART_PORT0, &ch, 1);
}