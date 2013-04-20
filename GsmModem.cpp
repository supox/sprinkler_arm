#include "GsmModem.h"
#include "CommunicationTimeouts.h"
#include "TimeManager.h"
#include "I2CRoutines.h"

#include <string.h>
#include <stdio.h>

GsmModem::GsmModem() : m_bHasInit(false)
{
	TextBufferInit(&ReadBuffer);
}

bool GsmModem::Init()
{
	if(!m_bHasInit) {
		I2C_LowLevel_Init();
		m_bHasInit = true;
	}
	return true;
}


void GsmModem::ResetModem()
{
	I2C_Master_Write("R", strlen("R"), MODEM_I2C1_CONTROL_ADDRESS);
	TimeManager::DelayMs(7000);
}

bool GsmModem::Write(const char* buffer)
{
	#ifdef _SIMULATOR
	printf("%s",buffer);
	#endif
	
	I2C_Master_Write((const unsigned char*)buffer, strlen(buffer), MODEM_I2C1_ADDRESS);
	
	return true;
}

void GsmModem::Write(const char ch)
{
	I2C_Master_Write((const unsigned char*)&ch, 1, MODEM_I2C1_ADDRESS);	
}

unsigned char GsmModem::NumberOfPendingByte(void)
{
		I2C_Master_Read(1, MODEM_I2C1_CONTROL_ADDRESS );
		if(TextBufferNumberOfAvailableBytes(I2C_GetReadingBuffer()))
			return TextBufferGetChar(I2C_GetReadingBuffer());
		return 0;
}

unsigned char GsmModem::ReadBytesFromModem()
{
		// Try to read new bytes from modem
		unsigned int number_of_pending_bytes = NumberOfPendingByte();
		if(number_of_pending_bytes > 0) { // Read new bytes from modem 
			if(number_of_pending_bytes>0xff)
				number_of_pending_bytes = 0xff;
			if(number_of_pending_bytes > TEXT_SIZE_OF_BUFFER)
				number_of_pending_bytes = TEXT_SIZE_OF_BUFFER;
			
			I2C_Master_Read( (unsigned char)number_of_pending_bytes , MODEM_I2C1_ADDRESS );
			TextBufferCopy( &ReadBuffer, I2C_GetReadingBuffer() );
		}
		return number_of_pending_bytes;
}

char* GsmModem::ReadLine(size_t timeout)
{
	char* response;
	int t0 = TimeManager::GetSystemTime();
	do
	{
		response = SearchForNewLine();
		if(response && response[0] != 0)
			return response;
		
		// Sleep for a while :
		TimeManager::DelayMs(20);
	}
	while(TimeManager::GetSystemTime() - t0 < timeout);
	return NULL;
}

static unsigned char LineBuffer[TEXT_SIZE_OF_BUFFER] = {0};
static unsigned char* LineBufferPtr = LineBuffer;
char* GsmModem::SearchForNewLine(void)
{
	// Try to read new bytes from modem
	if(ReadBytesFromModem() == 0 && ReadBuffer.NumberOfAvailableBytes == 0) // Could not read new bytes from modem 
		return NULL;
	
	// copy and search for new line:
	const unsigned char NumberOfBytes = ReadBuffer.NumberOfAvailableBytes;
	for(int byte_index = 0 ; byte_index < NumberOfBytes ; byte_index++)
	{
		const unsigned char CurrentByte = TextBufferGetChar(&ReadBuffer);
		switch(CurrentByte)
		{
			case '\r' :
			case 0 :
				// Ignore.
				break;
			case '\n':
				if(LineBufferPtr == LineBuffer)
					break; // empty line, ignore
				
				// else - end of line!
				*LineBufferPtr = 0; // End character string
				LineBufferPtr = LineBuffer; // Zero pointer for next writes
				return (char*)LineBuffer;
			default : 
				*LineBufferPtr = CurrentByte;
				LineBufferPtr++;
				if((LineBufferPtr - LineBuffer) >= TEXT_SIZE_OF_BUFFER) // Overflow
					LineBufferPtr = LineBuffer;
				break;
		}
	}
	// no new line has found, rewind and return null
	TextBufferRewindReadingPosition(&ReadBuffer, NumberOfBytes);
	return NULL;
}


void GsmModem::ClearBuffer()
{
	TimeManager::DelayMs(100);
	while(ReadBytesFromModem());
	TextBufferClear(&ReadBuffer);	
}


GsmModem& GsmModem::GetModem()
{
	static GsmModem instance;
	return instance;
}
