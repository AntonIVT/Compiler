#pragma once

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include "struct.hpp"

//--------------------------------------------------------------------

struct CodeBuffer
{
    char* buffer;
    int32_t size;
    int32_t offset;

    void construct(int32_t size);

    void printByte(BYTE command);
    
    void print4Byte(int32_t val);
    
    void print8Byte(int64_t val);

    /*  jmp/call 0x231323 .. <- src
         [] []   [][][][] []
    */
    void addRel4Byte(int32_t src, int32_t dest);

    void destruct();
};

//--------------------------------------------------------------------

void CodeBuffer::construct(int32_t size)
{
    buffer = (char*)calloc(size + 1, sizeof(char));
    assert(buffer != nullptr);

    this->size   = size;
    offset = 0;
}

//--------------------------------------------------------------------

void CodeBuffer::printByte(BYTE command)
{
    if (offset + sizeof(BYTE) >= size)
    {
        printf("Your program is too large can't compile\n");
        abort();
    }

    buffer[offset++] = command;
}

//--------------------------------------------------------------------

void CodeBuffer::print4Byte(int32_t val)
{
    if (offset + sizeof(int32_t) > size)
    {
        printf("Your program is too large can't compile\n");
        abort();
    }

    memcpy(buffer + offset, &val, sizeof(int32_t));
    offset += sizeof(int32_t);
}

//--------------------------------------------------------------------

void CodeBuffer::print8Byte(int64_t val)
{
    if (offset + sizeof(int64_t) > size)
    {
        printf("Your program is too large can't compile\n");
        abort();
    }

    memcpy(buffer + offset, &val, sizeof(int64_t));
    offset += sizeof(int64_t);
}

//--------------------------------------------------------------------

void CodeBuffer::addRel4Byte(int32_t src, int32_t dest)
{
    int32_t rel = dest - src;
    memcpy(buffer + src - 4, &rel, sizeof(int32_t));
}

//--------------------------------------------------------------------

void CodeBuffer::destruct()
{
    if (buffer != nullptr)
        free(buffer);
}
