#pragma once

#include <stddef.h>

namespace Storage
{
	// data_size should be multiple of 4. Writing would erase the current page.
	bool Write( unsigned int start_address, const unsigned char *data, unsigned int data_size );
	// data_size should be multiple of 4.
	bool Read( unsigned int start_address, unsigned char *data, unsigned int data_size );
	const unsigned char* Read(unsigned int start_address);
	// data_size should be mulitple of 1k.
	bool Erase( unsigned int start_address, unsigned int data_size );
	
	const size_t PageSize = 0x400; // 1024;
};
