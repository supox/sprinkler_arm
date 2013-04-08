#include "Logger.h"
#include <stdio.h>

void Logger::AddLine(const char* message, const Logger::LoggerLevel level)
{
	#ifdef _SIMULATOR
	printf("LOG : %s\n", message);
	#endif
}
