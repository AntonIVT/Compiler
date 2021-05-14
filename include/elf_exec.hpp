#pragma once

#include <cstdint>
#include <cstdio>
#include <elf.h>
#include <sys/stat.h>

//--------------------------------------------------------------------

/* Size: 64 byte */
struct ElfHeader
{
    uint32_t signature           = 0x464C457F;
    uint8_t  bit_mode            = ELFCLASS64;    // 64-bit 
    uint8_t  data_mode           = ELFDATA2LSB;   // Little endian
    uint8_t  version             = 0x01;
    uint8_t  os_abi              = ELFOSABI_NONE; // UNIX - System V
    uint8_t  abi_version         = 0x00; 
    uint8_t  unused[7]           = {0};
    uint16_t type                = ET_EXEC;
    uint16_t machine             = EM_X86_64;// Advanced Micro Devices X86-64
    uint32_t version2            = 0x01; 
    uint64_t entry_point         = 0x400080; // entry point
    uint64_t program_header      = 0x40;     // start of the program header table
    uint64_t section_header      = 0x00; 
    uint32_t flags               = 0x00;
    uint16_t header_size         = 0x40;
    uint16_t program_header_size = 0x38;
    uint16_t program_headers_num = 0x01;
    uint16_t section_header_size = 0x00;
    uint16_t section_header_num  = 0x00;
    uint16_t section_names       = 0x00;
};

//--------------------------------------------------------------------

/* Size: 64 byte */
struct ProgramHeader
{
    uint32_t type             = PT_LOAD;
    uint32_t flags            = PF_R | PF_X; // Read and execute
    uint64_t offset           = 0x00;
    uint64_t virtual_address  = 0x400000;
    uint64_t physical_address = 0x400000;
    uint64_t file_size        = 0x80;        
    uint64_t mem_size         = 0x80;
    uint64_t align            = 0x01;
    uint64_t unused           = 0x00;
};

//--------------------------------------------------------------------

void PrintElfInfo(FILE* file, int32_t program_size)
{
    assert(file != nullptr);

    ElfHeader elf;
    ProgramHeader prog;

    prog.file_size += program_size;
    prog.mem_size  += program_size;

    fwrite(&elf, sizeof(ElfHeader), 1, file);
    fwrite(&prog, sizeof(ProgramHeader), 1, file);
}
