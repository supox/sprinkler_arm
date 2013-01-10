#pragma once
#include <stdlib.h>

class StringBuffer
{
public:
	StringBuffer();
	~StringBuffer();

	size_t Write(const char* s, const size_t size);
	size_t Read(char* s, const size_t size);

	const char* GetBuffer() const {return memory;}
	size_t GetBufferSize() const {return write_pos;}
	
private:
	bool ReallocateBuffer(const size_t new_number_of_elements);

	char *memory;
	size_t write_pos;
	size_t read_pos;


};
