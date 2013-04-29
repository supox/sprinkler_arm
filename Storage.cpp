/******EEPROM .h*****/ 

#include "Storage.h"
#include "stm32f10x_flash.h"

#define FL_EERPOM_StartAddr 0x08007C00 // start from address after 94kbyte flash size 
#define FL_EEPROM_EndAddr 0x08007FFF // 1K data size 
#define FL_EEPROM_PageSize 0x400 // 1K 

namespace Storage
{
	bool Flash_EEPROM_ErasePage(unsigned int start_address, const unsigned int number_of_pages) ;
	
	/**** Function : Write the internal Flash data****/ 
	bool Write( unsigned int start_address, const unsigned char *data, unsigned int data_size ) 
	{
		static bool sHasErased = false;
		if(!sHasErased) {
			if(!Flash_EEPROM_ErasePage(0, 1))
				return false;
			sHasErased = true;
		}

		FLASH_Unlock(); 
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP| FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		
		start_address += FL_EERPOM_StartAddr;
		FLASH_Status FLASHStatus=FLASH_COMPLETE; 
		
		if((data_size) & 0x3 != 0)
			data_size += data_size & 0x3;

		const unsigned int *data_ptr = (const unsigned int*)data;
		const unsigned int *end_address = (const unsigned int*)(data + data_size);
		unsigned int current_address= start_address;
		for(; data_ptr < end_address ; current_address +=4, ++data_ptr) {
			FLASHStatus = FLASH_ProgramWord(current_address, *data_ptr); 
				if (FLASHStatus != FLASH_COMPLETE) 
					break;
		}
		FLASH_Lock(); 
		return FLASHStatus == FLASH_COMPLETE;
	}

	bool Flash_EEPROM_ErasePage(unsigned int start_address, const unsigned int number_of_pages) 
	{
		start_address += FL_EERPOM_StartAddr;
		
		FLASH_Unlock();
		FLASH_Status FLASHStatus=FLASH_COMPLETE; 

		/*Clear all the flash */ 
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR); 

		/*erase the page*/ 
		for(unsigned int erase_counter=0; erase_counter < number_of_pages; erase_counter++) 
		{ 
			FLASHStatus = FLASH_ErasePage(start_address + ( FL_EEPROM_PageSize * erase_counter )); 
			if (FLASHStatus != FLASH_COMPLETE) 
				break;
		} 

		FLASH_Lock(); 

		return FLASHStatus == FLASH_COMPLETE;
	} 

	bool Read( unsigned int start_address, unsigned char *data, unsigned int data_size ) 
	{
		start_address += FL_EERPOM_StartAddr;

		/* Copy, 4 bytes at a time */ 
		int* dest_data_ptr = (int*)data;
		const int* src_data_ptr = (const int*)start_address;
		const int* end_address = (const int*) (start_address + data_size);
		while( src_data_ptr <= end_address )
		{
			*(dest_data_ptr++) = *(src_data_ptr++);
		}
		return true;
	}
	
	const unsigned char* Read(unsigned int start_address)
	{
		start_address += FL_EERPOM_StartAddr;
		return (const unsigned char*)start_address;
	}

	///////////////////////////////////////////////////////////////////////////////// 
	/**************************************** 
	Function : format the internal Flash data 
	Input :- start address, end_address
	Output: return FLASHStatus; 
	****************************************/ 
	bool Erase( unsigned int start_adress, unsigned int data_size )
	{ 
		if(data_size <= 0)
			return true;
		
		unsigned int number_of_pages = data_size/FL_EEPROM_PageSize;
		if (number_of_pages< 1) 
			number_of_pages=1; 
		
		return Flash_EEPROM_ErasePage( start_adress, number_of_pages );
	}
};
