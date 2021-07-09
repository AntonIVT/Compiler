#pragma once
#include <elf.h>

typedef uint8_t BYTE;

//--------------------------------------------------------------------

typedef struct CodeBuffer
{
    char* buffer;
    int32_t size;
    int32_t offset;
} CodeBuffer;

void CodeBufferConstruct(CodeBuffer* cbuffer, int32_t size);

void CodeBufferWriteByte(CodeBuffer* cbuffer, BYTE command);

void CodeBufferWrite4Byte(CodeBuffer* cbuffer, int32_t val);

void CodeBufferWrite8Byte(CodeBuffer* cbuffer, int64_t val);

/*  jmp/call 0x231323 .. <- src
    [][]     [][][][] []
*/
void CodeBufferAddRel4Byte(CodeBuffer* cbuffer, int32_t src, int32_t dest);

void CodeBufferDestruct(CodeBuffer* cbuffer);
