#ifndef TEXT_BUFFER_H
#define	TEXT_BUFFER_H

#define TEXT_SIZE_OF_BUFFER (0x100)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char Buffer[TEXT_SIZE_OF_BUFFER];
    unsigned char* EndOfBuffer;
    unsigned char* StartOfBuffer;
    unsigned char* ReadingPosition;
    unsigned char* WritingPosition;
    unsigned int NumberOfAvailableBytes;
} TextBuffer;

void TextBufferInit(TextBuffer* buffer);
void TextBufferClear(TextBuffer* buffer);
void TextBufferPutChar(TextBuffer* buffer, const unsigned char ch);
void TextBufferWrite(TextBuffer* buffer, const unsigned char* message, unsigned int length);
void TextBufferCopy(TextBuffer* dest, TextBuffer* source);
unsigned char TextBufferGetChar(TextBuffer* buffer);
void TextBufferRewindReadingPosition(TextBuffer* buffer, unsigned int NumberOfBytes);
void TextBufferRewindWritingPosition(TextBuffer* buffer, unsigned int NumberOfBytes);
unsigned int TextBufferNumberOfAvailableBytes(TextBuffer* buffer);

#ifdef __cplusplus
}
#endif


#endif	/* TEXT_BUFFER_H */
