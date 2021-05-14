#pragma once

#include "tree.hpp"
#include "list.hpp"
#include "machine_code.hpp"
#include "code_buffer.hpp"
#include "elf_exec.hpp"
#include "doorsio.hpp"
#include "struct.hpp"

const int32_t MAX_BUFFER_SIZE = 100000;

//--------------------------------------------------------------------

struct FunctionCompiler
{
    List<Variable> name_table;
    List<Label>*   func_calls;
    CodeBuffer*    code;
    int32_t        var_offset;
    Node*          func_decl;
    
    void construct( Node* func_decl, CodeBuffer* code, List<Label>* func_calls);
    
    void destruct();
    
    int  get_offset(const char* variable);
    
    void push_call(Label call);

    void compile_func();
    
    void compile_func_args(Node* node, int curr_offset);
    
    void compile_exp_stmt_comp(Node *node);
    
    void compile_cond(Node *node);
    
    void compile_while(Node* node);
    
    void compile_ret(Node *node);
    
    void compile_print(Node *node);
    
    void compile_var_decl(Node* node);
    
    void compile_assgn(Node *node);

    void compile_expression(Node *node);
    
    int  compile_call_args(Node* node);
};

//--------------------------------------------------------------------

struct ProgramCompiler
{
    List<Label> functions;
    List<Label> func_calls;
    CodeBuffer  code;
    Tree*       tree;
    
    void construct(Tree *tree);
    
    void destruct();
    
    void print_calls();

    void compile_program(const char* out_file_name);
    
    void compile_func(Node* node);
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// ProgramCompiler methods
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void ProgramCompiler::construct(Tree *tree)
{    
    assert(tree != nullptr);

    this->tree = tree;
    
    functions.construct(1);
    func_calls.construct(1);
    code.construct(MAX_BUFFER_SIZE);
}

//--------------------------------------------------------------------

void ProgramCompiler::destruct()
{
    functions.destruct();
    func_calls.destruct();
    code.destruct();
}

//--------------------------------------------------------------------

void ProgramCompiler::print_calls()
{
    for (size_t i = 0; i < func_calls.get_size(); i++)
    {
        Label call = func_calls.get(i);

        bool is_exist = false;

        for (size_t j = 0; j < functions.get_size(); j++)
        {
            Label func = functions.get(j);

            if (!strcmp(call.name, func.name))
            {
                code.addRel4Byte(call.offset, func.offset);
                
                is_exist = true;
                break;
            }
        }

        if (!is_exist) printf("No label: %s\n", call.name);
    }
};

//--------------------------------------------------------------------

void ProgramCompiler::compile_program(const char* out_file_name)
{
    assert(out_file_name != nullptr);

    PrintCall(&code,  &func_calls, MAIN_FUNC);
    PrintMovRI(&code, RAX, 60);
    PrintMovRI(&code, RDI, 0);
    PrintSyscall(&code); 

    if (tree->root != nullptr)
        compile_func(tree->root);

    Stdlib lib = {};
    lib.construct("/home/anton/Documents/Repo/Compiler/stdoors/stdoors");
    
    functions.push_back(Label::Create("input",  lib.input_offset  + code.offset));
    functions.push_back(Label::Create("output", lib.output_offset + code.offset));
    functions.push_back(Label::Create("sqrt",   lib.sqrt_offset + code.offset));

    int32_t program_size = lib.size + code.offset;
    
    FILE* file = fopen(out_file_name, "wb");
    assert(file != nullptr);

    PrintElfInfo(file, program_size);

    print_calls();

    fwrite(code.buffer, sizeof(char), code.offset, file);
    fwrite(lib.code,    sizeof(char), lib.size,    file);

    lib.destruct();

    chmod(out_file_name, 0744);
}

//--------------------------------------------------------------------

void ProgramCompiler::compile_func(Node *node)
{
    if (node == nullptr || node->type != LINK)
        return;

    if (node->lnode != nullptr)
    {
        functions.push_back(Label::Create(node->lnode->value.strval, code.offset));
        FunctionCompiler func_translator = {};
        func_translator.construct(node->lnode, &code, &func_calls);
        func_translator.compile_func();
        func_translator.destruct();
    }

    if (node->rnode != nullptr)
        compile_func(node->rnode);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// FunctionCompiler methods
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void FunctionCompiler::construct(Node* func_decl, CodeBuffer* code, List<Label>* func_calls)
{
    assert( func_decl  != nullptr && code != nullptr &&
            func_calls != nullptr);

    this->func_decl  = func_decl;
    this->code       = code;
    this->func_calls = func_calls;

    var_offset = 0;

    name_table.construct(1);
}

//--------------------------------------------------------------------

void FunctionCompiler::destruct()
{
    name_table.destruct();
}

//--------------------------------------------------------------------

int FunctionCompiler::get_offset(const char *Variable)
{
    assert(Variable != nullptr);
    List_Iterator i = name_table.begin();
    assert(i != -1);

    do
    {
        if (!strcmp(name_table.get(i).name, Variable))
            return name_table.get(i).offset;

        i = name_table.iter_inc(i);

    } while (i != name_table.begin());

    printf("Unkown variable :<%s>\n", Variable);
    abort();
}

//--------------------------------------------------------------------

void FunctionCompiler::push_call(Label call)
{
    func_calls->push_back(call);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_func()
{
    assert(func_decl != nullptr && func_decl->type == FUN_DECL);
    
    PrintPush(code, RBP);
    PrintMovRR(code, RBP, RSP);

    /* ..., arg2, arg1, ret_adr, rbp */
    if (func_decl->lnode != nullptr)
        compile_func_args(func_decl->lnode, 16);

    compile_exp_stmt_comp(func_decl->rnode);

    PrintAddRI(code, RSP, -var_offset);
    PrintPop(code, RBP);

    PrintMovRI(code, RAX, 0);
    PrintRet(code);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_func_args(Node *node, int curr_offset)
{
    assert(node != nullptr && node->type == LINK);

    if (node->lnode != nullptr)
        name_table.push_back(Variable::Create(node->lnode->value.strval, curr_offset));

    if (node->rnode != nullptr)
        compile_func_args(node->rnode, curr_offset + 8);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_exp_stmt_comp(Node *node)
{
    assert(node != nullptr && node->type == LINK);

    if (node->lnode != nullptr)
    {
        switch (node->lnode->type)
        {
        case ASSGN_ST:
            compile_assgn(node->lnode);
            break;
        case RET_ST:
            compile_ret(node->lnode);
            break;
        case VAR_DECL:
            compile_var_decl(node->lnode);
            break;
        case PRINT:
            compile_print(node->lnode);
            break;
        case WHILE_ST:
            compile_while(node->lnode);
            break;
        case COND_ST:
            compile_cond(node->lnode);
            break;
        case FUN_CALL:
        {
            int arg_count = 0;
            if (node->lnode->lnode != nullptr)
                arg_count = compile_call_args(node->lnode->lnode);
            
            PrintCall(code, func_calls, node->lnode->value.strval);
            PrintAddRI(code, RSP, arg_count * 8);

            break;
        }
        default:
            break;
        }
    }
    if (node->rnode != nullptr)
        compile_exp_stmt_comp(node->rnode);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_cond(Node *node)
{
    assert( node        != nullptr && node->lnode        != nullptr &&
            node->rnode != nullptr && node->rnode->lnode != nullptr);

    compile_expression(node->lnode);

    PrintPop(code, RAX);
    PrintMovRI(code, RBX, 0);
    PrintCmpRR(code, RAX, RBX);
    PrintCondJump(code, JE);
    int32_t cond_false = code->offset;
    
    compile_exp_stmt_comp(node->rnode->lnode);

    PrintJmp(code);
    int32_t cond_end = code->offset;

    code->addRel4Byte(cond_false, code->offset);
    
    if (node->rnode->rnode != nullptr)
        compile_exp_stmt_comp(node->rnode->rnode);

    code->addRel4Byte(cond_end, code->offset);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_while(Node *node)
{
    assert(node != nullptr && node->lnode != nullptr && node->rnode != nullptr);

    int32_t while_begin = code->offset;
    
    compile_expression(node->lnode);
    
    PrintPop(code, RAX);
    PrintMovRI(code, RBX, 0);
    PrintCmpRR(code, RAX, RBX);
    PrintCondJump(code, JE);
    int32_t while_end = code->offset;
    
    compile_exp_stmt_comp(node->rnode);

    PrintJmp(code);
    code->addRel4Byte(code->offset, while_begin);

    code->addRel4Byte(while_end, code->offset);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_ret(Node *node)
{
    assert(node != nullptr);

    compile_expression(node->lnode);
    
    PrintPop(code, RAX);
    PrintAddRI(code, RSP, -var_offset);
    PrintPop(code, RBP);
    PrintRet(code);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_print(Node *node)
{
    assert(node != nullptr);

    compile_expression(node->lnode);
    
    PrintCall(code, func_calls, "output");
    PrintAddRI(code, RSP, 8);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_var_decl(Node *node)
{
    assert(node != nullptr);

    name_table.push_back(Variable::Create(node->value.strval, var_offset - 8));

    if (node->lnode != nullptr)
        compile_expression(node->lnode);
    else
    {
        PrintMovRI(code, RAX, rand());
        PrintPush(code, RAX);
    }

    var_offset -= 8;
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_assgn(Node *node)
{
    assert(node != nullptr && node->lnode != nullptr && node->lnode->type == IDENT);

    int32_t offset_tmp = get_offset(node->lnode->value.strval);

    compile_expression(node->rnode);

    if (node->value.ival != ASSGN)
        PrintMovRM(code, RAX, RBP, var_offset);

    switch (node->value.ival)
    {
    case ASSGN:
        PrintPop(code, RAX);
        PrintMovMR(code, RBP, RAX, var_offset);

        break;
    case ASSGN_A:
        PrintPop(code, RBX);
        PrintAddRR(code, RAX, RBX);

        break;
    case ASSGN_S:
        PrintPop(code, RBX);
        PrintSubRR(code, RAX, RBX);

        break;
    case ASSGN_M:
        PrintPop(code, RBX);
        PrintImulRR(code, RAX, RBX);

        PrintMovRI(code, RBX, 10000);
        PrintCqo(code);
        PrintIdivR(code, RBX);

        break;
    case ASSGN_D:
        PrintMovRI(code,  RCX, 10000);
        PrintImulRR(code, RAX, RCX);

        PrintPop(code, RBX);
        PrintCqo(code);
        PrintIdivR(code, RBX);

        break;
    default:
        break;
    }

    if (node->value.ival != ASSGN)
        PrintMovMR(code, RBP, RAX, var_offset);
}

//--------------------------------------------------------------------

#define PRINT_CMP(cond) PrintPop(code, RBX);        \
                        PrintPop(code, RAX);        \
                        PrintCmpRR(code, RAX, RBX); \
                        PrintSet(code, cond, RAX);  \
                        PrintMovzx(code, RAX, RAX); \
                        PrintPush(code, RAX);

void FunctionCompiler::compile_expression(Node *node)
{
    assert(node != nullptr);

    switch (node->type)
    {
    case NUM:
        
        PrintMovRI(code, RAX, (int64_t)(node->value.dval * 10000));
        PrintPush(code, RAX);
        
        break;
    case IDENT:
        
        PrintMovRM(code, RAX, RBP, get_offset(node->value.strval));
        PrintPush(code, RAX);
        
        break;
    case BIN_OPER:

        compile_expression(node->lnode);
        compile_expression(node->rnode);
        
        switch (node->value.ival)
        {
        case ADD:
            PrintPop(code, RBX);
            PrintPop(code, RAX);
            PrintAddRR(code, RAX, RBX);
            PrintPush(code, RAX);
            
            break;
        case SUB:
            PrintPop(code, RBX);
            PrintPop(code, RAX);
            PrintSubRR(code, RAX, RBX);
            PrintPush(code, RAX);

            break;
        case MUL:
            PrintPop(code, RBX);
            PrintPop(code, RAX);
            PrintImulRR(code, RAX, RBX);
            
            PrintMovRI(code, RBX, 10000);
            PrintCqo(code);
            PrintIdivR(code, RBX);
            
            PrintPush(code, RAX);            
            
            break;
        case DIV:
            PrintPop(code,    RBX);
            PrintPop(code,    RAX);
            PrintMovRI(code,  RCX, 10000);
            PrintImulRR(code, RAX, RCX);

            PrintCqo(code);
            PrintIdivR(code, RBX);
            
            PrintPush(code, RAX);

            break;
        case LESS:
            PRINT_CMP(SETL)
            break;
        case GREAT:
            PRINT_CMP(SETG)
            break;
        case LEQ:
            PRINT_CMP(SETLE)
            break;
        case GEQ:
            PRINT_CMP(SETGE)
            break;
        case EQ:
            PRINT_CMP(SETE)
            break;
        case NEQ:
            PRINT_CMP(SETNE)
            break;
        case OR:
            PrintPop(code, RBX);
            PrintPop(code, RAX);
            PrintOr(code, RAX, RBX);
            PrintPush(code, RAX);

            break;
        case AND:
            PrintPop(code, RBX);
            PrintPop(code, RAX);
            PrintAnd(code, RAX, RBX);
            PrintPush(code, RAX);

            break;
        default:
            printf("Unknown binary operation\n");
            break;
        }
        break;
    case INPUT:

        PrintCall(code, func_calls, "input");
        PrintPush(code, RAX);

        break;
    case FUN_CALL:
    {
        int arg_count = 0;
        if (node->lnode != nullptr)
            arg_count = compile_call_args(node->lnode);
        
        PrintCall(code, func_calls, node->value.strval);
        PrintAddRI(code, RSP, arg_count * 8);
        PrintPush(code, RAX);

        break;
    }
    default:
        break;
    }
}

#undef PRINT_CMP

//--------------------------------------------------------------------

int FunctionCompiler::compile_call_args(Node *node)
{
    if (node == nullptr)
        printf("nullptr\n");
    if (node->type != LINK)
        printf("%i\n", node->type);
    
    assert(node != nullptr && node->type == LINK);

    int arg_count = 0;

    if (node->rnode != nullptr)
        arg_count = compile_call_args(node->rnode);

    if (node->lnode != nullptr)
        compile_expression(node->lnode);

    return arg_count + 1;
}

//--------------------------------------------------------------------
