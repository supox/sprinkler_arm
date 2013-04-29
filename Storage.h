#pragma once

#include <stddef.h>

namespace Storage
{
	// data_size should be multiple of 4.
	bool Write( unsigned int start_address, const unsigned char *data, unsigned int data_size );
	// data_size should be multiple of 4.
	bool Read( unsigned int start_adress, unsigned char *data, unsigned int data_size );

	// data_size should be mulitple of 1k.
	bool Erase( unsigned int start_adress, unsigned int data_size );
};
