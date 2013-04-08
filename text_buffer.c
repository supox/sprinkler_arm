#include "text_buffer.h"
#include <string.h>

void TextBufferInit(TextBuffer* buffer) {
    buffer->EndOfBuffer = buffer->Buffer + TEXT_SIZE_OF_BUFFER;
    buffer->StartOfBuffer = buffer->Buffer;
    TextBufferClear(buffer);
}

void TextBufferClear(TextBuffer* buffer) {
    buffer->ReadingPosition = buffer->StartOfBuffer;
    buffer->WritingPosition = buffer->StartOfBuffer;
    buffer->NumberOfAvailableBytes = 0;
}

void TextBufferPutChar(TextBuffer* buffer, const unsigned char ch) {
    *buffer->WritingPosition = ch;
    buffer->WritingPosition++;
    if (buffer->WritingPosition == buffer->EndOfBuffer)
        buffer->WritingPosition = buffer->StartOfBuffer;
    if (buffer->NumberOfAvailableBytes < TEXT_SIZE_OF_BUFFER)
        buffer->NumberOfAvailableBytes++;
}

void TextBufferWrite(TextBuffer* buffer, const unsigned char* message, unsigned int length)
{
    while(length--) {
        TextBufferPutChar(buffer, *message);
        message++;
    }
}

unsigned char TextBufferGetChar(TextBuffer* buffer) {
    unsigned char ch = *buffer->ReadingPosition;
    if (buffer->NumberOfAvailableBytes) {
        buffer->ReadingPosition++;
        if (buffer->ReadingPosition == buffer->EndOfBuffer)
            buffer->ReadingPosition = buffer->StartOfBuffer;
        buffer->NumberOfAvailableBytes--;
    }
    // else - error
    return ch;
}

void TextBufferRewindReadingPosition(TextBuffer* buffer, unsigned int NumberOfBytes)
{
    unsigned int NumberOfAvailableBytes = buffer->NumberOfAvailableBytes + NumberOfBytes;
    if(NumberOfAvailableBytes >= TEXT_SIZE_OF_BUFFER)
        NumberOfAvailableBytes = TEXT_SIZE_OF_BUFFER - 1;
		NumberOfBytes = NumberOfAvailableBytes - buffer->NumberOfAvailableBytes ;
		
    buffer->NumberOfAvailableBytes = NumberOfAvailableBytes;

    while(NumberOfBytes--)
    {
        buffer->ReadingPosition--;
        if( buffer->ReadingPosition < buffer->StartOfBuffer)
            buffer->ReadingPosition = buffer->EndOfBuffer-1;
    }
}

void TextBufferRewindWritingPosition(TextBuffer* buffer, unsigned int NumberOfBytes)
{
    int NumberOfAvailableBytes = buffer->NumberOfAvailableBytes - NumberOfBytes;
    if(NumberOfAvailableBytes < 0)
        NumberOfAvailableBytes = 0;
		NumberOfBytes = buffer->NumberOfAvailableBytes - NumberOfAvailableBytes;
		
    buffer->NumberOfAvailableBytes = NumberOfAvailableBytes;

    while(NumberOfBytes--)
    {
        buffer->WritingPosition--;
        if( buffer->WritingPosition < buffer->StartOfBuffer)
            buffer->WritingPosition = buffer->EndOfBuffer-1;
    }
}

unsigned int TextBufferNumberOfAvailableBytes(TextBuffer* buffer) {
    return buffer->NumberOfAvailableBytes;
}

void TextBufferCopy(TextBuffer* dest, TextBuffer* source)
{
	TextBufferClear(dest);
	if( source->ReadingPosition < source->WritingPosition ) {
		memcpy(dest->Buffer, source->ReadingPosition, source->NumberOfAvailableBytes);
		dest->NumberOfAvailableBytes = source->NumberOfAvailableBytes;
		dest->WritingPosition += source->NumberOfAvailableBytes;
	}
	else {
		const size_t NumberOfBytes = source->NumberOfAvailableBytes;
		while(source->NumberOfAvailableBytes)
			TextBufferPutChar(dest, TextBufferGetChar(source));
		TextBufferRewindReadingPosition(source, NumberOfBytes);			
	}
}
