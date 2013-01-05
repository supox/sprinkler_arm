#ifdef _SIMULATOR

	#include "STM32F10x.h"
	#include <stdio.h>

	struct __FILE { int handle; /* Add whatever you need here */ };
	FILE __stdout;
	// FILE __stdin;
	int fputc(int ch, FILE *f) {
		ITM_SendChar(ch);
		return(ch);
	}

#endif
