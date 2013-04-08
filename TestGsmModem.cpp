#ifdef _TEST_GSM_MODEM

#include "GsmModem.h"
#include "Tests.h"
#include "StringBuffer.h"
#include "TestHelpers.h"
#include "TimeManager.h"

using namespace TestHelpers;

void TestGsmModem()
{
	const size_t number_of_cycles = 1;
	const size_t bytes_to_transmit = 0x1A;
	const size_t number_of_reading = 10;
	
	
	for(size_t cycle_index = 0 ; cycle_index < number_of_cycles ; cycle_index++) {
		StringBuffer sb;
		GsmModem &modem = GsmModem::GetModem();	
		modem.Init();
		
		TextBufferClear(&modem.ReadBuffer);

		// poll bytes
		for(int i=0; i < number_of_reading ; i++)
		{
			if(modem.ReadBytesFromModem()) {
				assert(modem.ReadBuffer.NumberOfAvailableBytes == bytes_to_transmit);
				sb.Write((char*)modem.ReadBuffer.ReadingPosition, modem.ReadBuffer.NumberOfAvailableBytes);
				TextBufferClear(&modem.ReadBuffer);
			}
			else
				assert(false);
			TimeManager::DelayMs(5);
		}
		
		assert(sb.GetBufferSize() == number_of_reading * bytes_to_transmit);
		
		unsigned char current_char = 1;
		const unsigned char* buffer_ptr = (const unsigned char*)sb.GetBuffer();
		for(int i = 0 ; i < number_of_reading ; i++) {
			for(unsigned char j = 0 ; j < bytes_to_transmit ; j++) {
				if(*buffer_ptr != current_char)
					assert(*buffer_ptr != current_char);
				buffer_ptr++;
				current_char++;
				if(current_char > 0x80) 
					current_char = 1;
			}
		}
	}
}

#endif
