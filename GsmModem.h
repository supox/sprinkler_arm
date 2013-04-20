#pragma once
#include "text_buffer.h"
#include <stdlib.h>

#ifdef _TEST_GSM_MODEM
#	define MODEM_I2C1_ADDRESS	 (0x16)
#else
#	define MODEM_I2C1_ADDRESS	 (0x12)
#endif

#define MODEM_I2C1_CONTROL_ADDRESS (MODEM_I2C1_ADDRESS + 2)

class GsmModem
{
public:
	static GsmModem& GetModem();
	TextBuffer ReadBuffer;
	
	bool Init();
	bool Write(const char* buffer);
	void Write(const char ch);
	unsigned char NumberOfPendingByte(void);
	unsigned char ReadBytesFromModem();
	char* ReadLine(size_t timeout);
	void ClearBuffer();
	void ResetModem();

private:
	GsmModem();

	char* SearchForNewLine(void);

	bool m_bHasInit;
};
