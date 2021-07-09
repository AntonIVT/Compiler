#pragma once

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "struct.hpp"
#include "code_buffer.hpp"
#include "list.hpp"

//--------------------------------------------------------------------

enum Registers
{
    RAX = 0b000,
    RCX = 0b001,
    RDX = 0b010,
    RBX = 0b011,
    RSP = 0b100,
    RBP = 0b101,
    RSI = 0b110,
    RDI = 0b111,
};

//--------------------------------------------------------------------

enum Jumps
{
    JE  = 0x84,
    JNE = 0x85,
    JL  = 0x8C,
    JG  = 0x8F,
    JLE = 0x8E,
    JGE = 0x8D,
};

//--------------------------------------------------------------------

enum Sets
{
    SETE  = 0x94,
    SETNE = 0x95,
    SETL  = 0x9C,
    SETG  = 0x9F,
    SETLE = 0x9E,
    SETGE = 0x9D
};

//--------------------------------------------------------------------

const uint32_t plug = 0xDA0EDADE;

/* Default code for 64 bit operand size */
const BYTE def_64 = 0x48;

/* Simple code operator */
const BYTE mov_rr  = 0x89;
const BYTE mov_rm  = 0x8B;
const BYTE mov_mr  = 0x89;

const BYTE cmp_rr  = 0x39;
const BYTE and_rr  = 0x21;
const BYTE or_rr   = 0x09;

const BYTE add_rr  = 0x01;
const BYTE sub_rr  = 0x29;
const BYTE imul_rr = 0xAF;

const BYTE syscall = 0x05;
const BYTE ret     = 0xC3;
const BYTE jmp_32  = 0xE9;
const BYTE call_32 = 0xE8;

const BYTE movzx   = 0xB6;
const BYTE cqo     = 0x99;
/* Double byte commands */
const BYTE double_byte = 0x0F;

/* (Code + R) code operator */
const BYTE mov_ri = 0xB8;
const BYTE push_r = 0x50;
const BYTE pop_r  = 0x58;

const BYTE add_ri     = 0x81;
const BYTE add_ri_arg = 0xC0;

const BYTE idiv_r     = 0xF7;
const BYTE idiv_r_arg = 0xF8;

/* Extended bits for registers code */
const BYTE ext_rr = 0b11;
const BYTE ext_rm = 0b10;

/* Work with memory and rsp */
const BYTE rsp_mem = 0x24;

//--------------------------------------------------------------------

/* Get two registers argument */
BYTE GetRR(BYTE ext, BYTE reg1, BYTE reg2)
{  
    return ((ext << 6) | (reg1 << 3) | reg2); 
}

//--------------------------------------------------------------------

/* mov reg1, reg2 */
void EmitMovRR(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(mov_rr);
    buffer->printByte(GetRR(ext_rr, reg2, reg1));
}

//--------------------------------------------------------------------

/* mov reg1, [reg2 + offset] */
void EmitMovRM(CodeBuffer* buffer, BYTE reg1, BYTE reg2, int32_t offset)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(mov_rm);
    buffer->printByte(GetRR(ext_rm, reg1, reg2));

    if (reg2 == RSP)
        buffer->printByte(rsp_mem);
    
    buffer->print4Byte(offset);
}

//--------------------------------------------------------------------

/* mov [reg1 + offset], reg2 */
void EmitMovMR(CodeBuffer* buffer, BYTE reg1, BYTE reg2, int32_t offset)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(mov_mr);
    buffer->printByte(GetRR(ext_rm, reg2, reg1));

    if (reg1 == RSP)
        buffer->printByte(rsp_mem);
    
    buffer->print4Byte(offset);
}

//--------------------------------------------------------------------

/* mov reg, imm */
void EmitMovRI(CodeBuffer* buffer, BYTE registr, int64_t imm)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(mov_ri + registr);
    buffer->print8Byte(imm);
}

//--------------------------------------------------------------------

/* push reg */
void EmitPush(CodeBuffer* buffer, BYTE registr)
{
    assert(buffer != nullptr);

    buffer->printByte(push_r + registr);
}

//--------------------------------------------------------------------

/* pop reg */
void EmitPop(CodeBuffer* buffer, BYTE registr)
{
    assert(buffer != nullptr);

    buffer->printByte(pop_r + registr);    
}

//--------------------------------------------------------------------

/* ret */
void EmitRet(CodeBuffer* buffer)
{
    assert(buffer != nullptr);

    buffer->printByte(ret);
}

//--------------------------------------------------------------------

/* add reg, imm */
void EmitAddRI(CodeBuffer* buffer, BYTE registr, int imm)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(add_ri);
    buffer->printByte(add_ri_arg + registr);
    buffer->print4Byte(imm);
}

//--------------------------------------------------------------------

/* cmp reg1, reg2 */
void EmitCmpRR(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(cmp_rr);
    buffer->printByte(GetRR(ext_rr, reg2, reg1));
}

//--------------------------------------------------------------------

/* add reg1, reg2 */
void EmitAddRR(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(add_rr);
    buffer->printByte(GetRR(ext_rr, reg2, reg1));
}

//--------------------------------------------------------------------

/* sub reg1, reg2 */
void EmitSubRR(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(sub_rr);
    buffer->printByte(GetRR(ext_rr, reg2, reg1));
}

//--------------------------------------------------------------------

/* imul reg1, reg2 */
void EmitImulRR(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(double_byte);
    buffer->printByte(imul_rr);
    buffer->printByte(GetRR(ext_rr, reg1, reg2));
}

//--------------------------------------------------------------------

/* idiv reg */
void EmitIdivR(CodeBuffer* buffer, BYTE reg)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(idiv_r);
    buffer->printByte(idiv_r_arg + reg);
}

//--------------------------------------------------------------------

/* syscall */
void EmitSyscall(CodeBuffer* buffer)
{
    assert(buffer != nullptr);

    buffer->printByte(double_byte);
    buffer->printByte(syscall);
}

//--------------------------------------------------------------------

/* jmp *plug* */
void EmitJmp(CodeBuffer* buffer)
{
    assert(buffer != nullptr);

    buffer->printByte(jmp_32);
    buffer->print4Byte(plug);    
}

//--------------------------------------------------------------------

/* call label */
void EmitCall(CodeBuffer* buffer, List<Label>* calls, const char* label)
{
    assert( buffer != nullptr &&
            calls  != nullptr &&
            label  != nullptr);

    buffer->printByte(call_32);
    buffer->print4Byte(plug);

    calls->push_back(Label::Create(label, buffer->offset));
}

//--------------------------------------------------------------------

/* j(cond) plug */
void EmitCondJump(CodeBuffer* buffer, BYTE cond)
{
    assert(buffer != nullptr);
    
    buffer->printByte(double_byte);
    buffer->printByte(cond);
    buffer->print4Byte(plug);
}

//--------------------------------------------------------------------

/* set(cond) reg */
void EmitSet(CodeBuffer* buffer, BYTE cond, BYTE reg)
{
    assert(buffer != nullptr);

    buffer->printByte(double_byte);
    buffer->printByte(cond);
    buffer->printByte(add_ri_arg + reg);
}

//--------------------------------------------------------------------

/* movzx reg1, reg2 */
void EmitMovzx(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(double_byte);
    buffer->printByte(movzx);
    buffer->printByte(GetRR(ext_rr, reg1, reg2));
}

//--------------------------------------------------------------------

/* and reg1, reg2 */
void EmitAnd(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(and_rr);
    buffer->printByte(GetRR(ext_rr, reg2, reg1));
}

//--------------------------------------------------------------------

/* or reg1, reg2 */
void EmitOr(CodeBuffer* buffer, BYTE reg1, BYTE reg2)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(or_rr);
    buffer->printByte(GetRR(ext_rr, reg2, reg1));
}

void EmitCqo(CodeBuffer* buffer)
{
    assert(buffer != nullptr);

    buffer->printByte(def_64);
    buffer->printByte(cqo);
}
