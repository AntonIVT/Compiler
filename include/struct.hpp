#pragma once

#include <cassert>
#include <cstdlib>
#include <elf.h>

typedef uint8_t BYTE;
typedef uint8_t REG;

const char* MAIN_FUNC = "start";

struct Label
{
    const char* name;
    int32_t offset;

    static Label Create(const char* name, int offset)
    {
        assert(name != nullptr);

        Label tmp = {name, offset};
        return tmp;
    }
};

struct Variable
{
    const char *name;
    int offset;
    
    static Variable Create(const char *name, int offset)
    {
        assert(name != nullptr);

        Variable tmp = {name, offset};
        return tmp;
    }
};
