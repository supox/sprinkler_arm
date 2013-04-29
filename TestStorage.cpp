#ifdef _TESTS_STORAGE
#include "TestHelpers.h"
#include "Storage.h"

#include <string.h>

using namespace TestHelpers;

void TestStorage()
{
	unsigned int start_address = 0x20;
	unsigned int start_address2 = 0x40;
	const unsigned char data_source[] = "Hello world and flash..........";
	const unsigned char data_source2[] = "Yeah...";
	bool ret ;
	// Test Write:
	ret = Storage::Write( start_address, data_source, sizeof(data_source));
	assert(ret);
	ret = Storage::Write( start_address2, data_source2, sizeof(data_source2));
	assert(ret);
	
	// Test Read
	unsigned char data_dest[0x40];
	unsigned char data_dest2[0x20];
	ret = Storage::Read( start_address, data_dest, sizeof(data_source) );
	assert(ret);
	assert(strcmp((const char*)data_source, (const char*)data_dest)==0);
	assert(strcmp((const char*)data_source, (const char*)Storage::Read(start_address))==0);
	ret = Storage::Read( start_address2, data_dest2, sizeof(data_source2) );
	assert(ret);
	assert(strcmp((const char*)data_source2, (const char*)data_dest2)==0);
	assert(strcmp((const char*)data_source2, (const char*)Storage::Read(start_address2))==0);
	
	// Test Erase
	ret = Storage::Erase( start_address, sizeof(data_source));
	assert(ret);
	assert(strcmp((const char*)data_source, (const char*)Storage::Read(start_address))!=0);
}

#endif
