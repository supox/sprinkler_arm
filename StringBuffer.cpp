#include "StringBuffer.h"
#include "Logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


StringBuffer::StringBuffer() :
	memory(NULL), write_pos(0), read_pos(0)
{
	memory = (char*)malloc(1);
	memory[0] = 0;
}

StringBuffer::~StringBuffer()
{
	free(memory);
}

bool StringBuffer::ReallocateBuffer(const size_t new_number_of_elements)
{
    char* new_buf = (char*)realloc(memory, sizeof(char) * new_number_of_elements);
    if(new_buf != NULL) {
        memory = new_buf;
        return true;
    }
    return false;
}

size_t StringBuffer::Write(const char* s, const size_t size)
{
   if(!ReallocateBuffer(write_pos + size + 1)) {
        /* out of memory! */
        Logger::AddLine("not enough memory (realloc returned NULL)", Logger::ERROR);
        return 0;
    }

    memcpy(&(memory[write_pos]), s, size);
    write_pos += size;
    memory[write_pos] = 0;

    return size;
}

size_t StringBuffer::Read(char* s, const size_t size)
{
    const size_t bytes_left = write_pos - read_pos;
    size_t realsize = size;
    if (realsize > bytes_left)
        realsize = bytes_left;

    if (realsize > 0) {
        memcpy(s, &(memory[read_pos]), realsize);
        read_pos += realsize;
    }

    return realsize;
}
