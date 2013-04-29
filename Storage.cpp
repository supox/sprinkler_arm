/******EEPROM .h*****/ 

#include "Storage.h"
#include "stm32f10x_flash.h"

#define FL_EERPOM_StartAddr 0x08017000 // start from address after 94kbyte flash size 
#define FL_EEPROM_EndAddr 0x08017800 // 2K data size 
#define FL_EEPROM_PageSize 0x400 // 1K 

namespace Storage
{
	/**** Function : Write the internal Flash data****/ 
	bool Write( unsigned int start_address, const unsigned char *data, unsigned int data_size ) 
	{ 
		start_address += FL_EERPOM_StartAddr;
		FLASH_Status FLASHStatus=FLASH_COMPLETE; 
		
		if((data_size) & 0x3 != 0)
			data_size += data_size & 0x3;

		FLASH_Unlock(); 

		const unsigned int end_address = start_address + data_size; 
		
		for(Flash_EEPROM_Addr = start_address; Flash_EEPROM_Addr <  end_address ; Flash_EEPROM_Addr +=4, data += 4) {
			FLASHStatus = FLASH_ProgramWord(Flash_EEPROM_Addr, *data); 
				if (FLASHStatus != FLASH_COMPLETE) 
					break;
		}
		FLASH_Lock(); 
		return FLASHStatus = FLASH_COMPLETE;
	}

	bool Flash_EEPROM_ErasePage(unsigned int start_adress, const unsigned int number_of_pages) 
	{
		start_address += FL_EERPOM_StartAddr;
		
		FLASH_Unlock();
		FLASH_Status FLASHStatus=FLASH_COMPLETE; 

		/*Clear all the flash */ 
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR); 

		/*erase the page*/ 
		for(erase_counter=0; erase_counter < number_of_pages; erase_counter++) 
		{ 
			FLASHStatus = FLASH_ErasePage(start_adress + ( FL_EEPROM_PageSize * erase_counter )); 
			if (FLASHStatus != FLASH_COMPLETE) 
				break;
		} 

		FLASH_Lock(); 

		return FLASHStatus = FLASH_COMPLETE;
	} 

	bool Read( unsigned int start_adress, unsigned char *data, unsigned int data_size ) 
	{
		start_address += FL_EERPOM_StartAddr;
		FLASH_Status FLASHStatus=FLASH_COMPLETE; 

		/* Copy, 4 bytes at a time */ 
		int* dest_data_ptr = (int*)data;
		int* src_data_ptr = (int*)start_address;
		int* end_address = (int*) (start_adress + data_size);
		while( src_data_ptr <= end_address )
		{
			*(dest_data_ptr++) = *(src_data_ptr++);
		}
		FLASH_Lock(); 
		return FLASHStatus = FLASH_COMPLETE;
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
