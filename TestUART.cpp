#ifdef _TESTS
#include "STM32F10x.h"

static int getch()
{
	/* Wait until the data is ready to be received. */
	while ((USART1->SR & USART_SR_RXNE) == 0);

	// read RX data, combine with DR mask (we only accept a max of 9 Bits)
	return USART1->DR & 0x1FF;	
}

static void putch(const int data)
{
	USART1->DR = data;

	// wait for TX
	while ((USART1->SR & USART_SR_TXE) == 0);	
}

static void putstr(const char* data)
{
	for(int i = 0 ; data[i] ; i++)
		putch(data[i]);
}

void CheckUART()
{
	RCC->APB2ENR = 0
			// Turn on USART1
			| RCC_APB2ENR_USART1EN
			// Turn on IO Port A
			| RCC_APB2ENR_IOPAEN;
	// Put PA9  (TX) to alternate function output push-pull at 50 MHz
	// Put PA10 (RX) to floating input
	GPIOA->CRH = 0x000004B0;
	// Configure BRR by deviding the bus clock with the baud rate
	USART1->BRR = 72000000/115200;

	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	
	putstr("\n\rHello, please type something\n\r");

	while(true) {
		// read blocking form usart
		int rec = getch();

		if(rec == '\r') {
				putstr("\n\r");
		} else {
				// send data blocking to usart
				putch('[');
				putch(rec);
				putch(']');
		}
	}
}

#endif
