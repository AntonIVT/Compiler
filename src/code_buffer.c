#include "code_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//--------------------------------------------------------------------

void CodeBufferConstruct(CodeBuffer* cbuffer, int32_t size)
{
    cbuffer->buffer = (char*)calloc(size + 1, sizeof(char));
    assert(cbuffer->buffer != nullptr);

    cbuffer->size   = size;
    cbuffer->offset = 0;
}

//--------------------------------------------------------------------

void CodeBufferPrintByte(CodeBuffer* cbuffer, BYTE command)
{
    if (cbuffer->offset + sizeof(BYTE) >= cbuffer->size)
    {
        printf("Your program is too large can't compile\n");
        abort();
    }

    cbuffer->buffer[cbuffer->offset++] = command;
}

//--------------------------------------------------------------------

void CodeBufferPrint4Byte(CodeBuffer* cbuffer, int32_t val)
{
    if (cbuffer->offset + sizeof(int32_t) >= cbuffer->size)
    {
        printf("Your program is too large can't compile\n");
        abort();
    }

    memcpy(cbuffer->buffer + cbuffer->offset, &val, sizeof(int32_t));
    cbuffer->offset += sizeof(int32_t);
}

//--------------------------------------------------------------------

void CodeBufferPrint8Byte(CodeBuffer* cbuffer, int64_t val)
{
    if (cbuffer->offset + sizeof(int64_t) >= cbuffer->size)
    {
        printf("Your program is too large can't compile\n");
        abort();
    }

    memcpy(cbuffer->buffer + cbuffer->offset, &val, sizeof(int64_t));
    cbuffer->offset += sizeof(int64_t);
}

//--------------------------------------------------------------------

void CodeBufferAddRel4Byte(CodeBuffer* cbuffer, int32_t src, int32_t dest)
{
    int32_t rel = dest - src;
    memcpy(cbuffer->buffer + src - 4, &rel, sizeof(int32_t));
}

//--------------------------------------------------------------------

void CodeBufferDestruct(CodeBuffer* cbuffer)
{
    if (cbuffer->buffer != NULL)
        free(cbuffer->buffer);
}
