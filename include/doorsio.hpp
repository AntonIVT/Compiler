#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

//--------------------------------------------------------------------

const char STD_BEGIN[]  = "START_OF_THE_STDLIB";
const char STD_INPUT[]  = "INPUT_STDLIB";
const char STD_OUTPUT[] = "OUTPUT_STDLIB";
const char STD_SQRT[]   = "SQRT_STDLIB";
const char STD_END[]    = "END_OF_THE_STDLIB";

//--------------------------------------------------------------------

struct Stdlib
{
    char* code;
    int32_t size;
    
    int32_t input_offset;
    int32_t output_offset;
    int32_t sqrt_offset;

    void construct(const char* stdlib_binary_filename);

    void destruct();
};

//--------------------------------------------------------------------

int GetFileSize(FILE *file)
{
    assert(file != NULL);

	int _file_size = -1;

	fseek(file, 0, SEEK_END); //Puts pointer in file to the end
	_file_size = ftell(file); //ftell() checks how many bytes from start of file to the pointer on file
    fseek(file, 0, SEEK_SET);

	return _file_size;
}

//--------------------------------------------------------------------

const char* Strstr(const char* source, int32_t source_size, const char* sub_str)
{
    int32_t sub_str_ptr = -1;
    int32_t source_ptr = 0;

    while(source_ptr < source_size)
    {
        /* Prev char doesn't match */
        if (sub_str_ptr == -1)
        {
            if (source[source_ptr] == sub_str[0])
                sub_str_ptr = 1;
        }
        else
        {
            if (source[source_ptr] == sub_str[sub_str_ptr])
            {
                sub_str_ptr++;
                if (sub_str[sub_str_ptr] == '\0')
                    return (source + (source_ptr - (sub_str_ptr - 1)));
            }
            else
                sub_str_ptr = -1;
        }
        source_ptr++;
    }

    return nullptr;
}

//--------------------------------------------------------------------

void Stdlib::construct(const char* lib_binary)
{
    assert(lib_binary != nullptr);

    FILE* std_file = fopen(lib_binary, "rb");
    assert(std_file != nullptr);

    int file_size = GetFileSize(std_file);
    assert(file_size >= 0);

    char* buffer = (char*)calloc(file_size + 1, sizeof(char));
    assert(buffer != nullptr);
    fread(buffer, sizeof(char), file_size, std_file);

    const char* begin  = Strstr(buffer, file_size, STD_BEGIN);
    const char* input  = Strstr(buffer, file_size, STD_INPUT);
    const char* output = Strstr(buffer, file_size, STD_OUTPUT);
    const char* sqrt   = Strstr(buffer, file_size, STD_SQRT);  
    const char* end    = Strstr(buffer, file_size, STD_END);
    
    assert(begin  != nullptr &&
           input  != nullptr &&
           output != nullptr &&
           end    != nullptr &&
           sqrt   != nullptr);

    int32_t begin_offset = begin - buffer + (sizeof(STD_BEGIN) - 1);
    
    input_offset  = input  - buffer + (sizeof(STD_INPUT)  - 1) - begin_offset;
    output_offset = output - buffer + (sizeof(STD_OUTPUT) - 1) - begin_offset;
    sqrt_offset   = sqrt   - buffer + (sizeof(STD_SQRT)   - 1) - begin_offset;

    size = (end - buffer) - begin_offset;

    code = (char*)calloc(size + 1, sizeof(char));
    assert(code != nullptr);

    memcpy(code, buffer + begin_offset, size);
    code[size] = '\0';
    free(buffer);
}


void Stdlib::destruct()
{
    if (code != nullptr)
        free(code);
}
