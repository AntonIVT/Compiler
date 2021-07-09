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
    bool           optimization;
    
    void construct(Node* func_decl, CodeBuffer* code, List<Label>* func_calls);
    
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
    bool        optimization;
    
    void construct(Tree *tree);
    
    void destruct();
    
    void print_calls();

    void compile_program(const char* out_file_name, bool optimization);
    
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

void ProgramCompiler::compile_program(const char* out_file_name, bool optimization)
{
    assert(out_file_name != nullptr);

    EmitCall(&code,  &func_calls, MAIN_FUNC);
    EmitMovRR(&code, RDI, RAX);
    EmitMovRI(&code, RAX, 60);
    EmitSyscall(&code); 

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
    
    EmitPush(code, RBP);
    EmitMovRR(code, RBP, RSP);

    /* ..., arg2, arg1, ret_adr, rbp */
    if (func_decl->lnode != nullptr)
        compile_func_args(func_decl->lnode, 16);

    compile_exp_stmt_comp(func_decl->rnode);

    EmitAddRI(code, RSP, -var_offset);
    EmitPop(code, RBP);

    EmitMovRI(code, RAX, 0);
    EmitRet(code);
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
            
            EmitCall(code, func_calls, node->lnode->value.strval);
            EmitAddRI(code, RSP, arg_count * 8);

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

    EmitPop(code, RAX);
    EmitMovRI(code, RBX, 0);
    EmitCmpRR(code, RAX, RBX);
    EmitCondJump(code, JE);
    int32_t cond_false = code->offset;
    
    compile_exp_stmt_comp(node->rnode->lnode);

    EmitJmp(code);
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
    
    EmitPop(code, RAX);
    EmitMovRI(code, RBX, 0);
    EmitCmpRR(code, RAX, RBX);
    EmitCondJump(code, JE);
    int32_t while_end = code->offset;
    
    compile_exp_stmt_comp(node->rnode);

    EmitJmp(code);
    code->addRel4Byte(code->offset, while_begin);

    code->addRel4Byte(while_end, code->offset);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_ret(Node *node)
{
    assert(node != nullptr);

    compile_expression(node->lnode);
    
    EmitPop(code, RAX);
    EmitAddRI(code, RSP, -var_offset);
    EmitPop(code, RBP);
    EmitRet(code);
}

//--------------------------------------------------------------------

void FunctionCompiler::compile_print(Node *node)
{
    assert(node != nullptr);

    compile_expression(node->lnode);
    
    EmitCall(code, func_calls, "output");
    EmitAddRI(code, RSP, 8);
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
        EmitMovRI(code, RAX, rand());
        EmitPush(code, RAX);
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
        EmitMovRM(code, RAX, RBP, offset_tmp);

    switch (node->value.ival)
    {
    case ASSGN:
        EmitPop(code, RAX);
        EmitMovMR(code, RBP, RAX, offset_tmp);

        break;
    case ASSGN_A:
        EmitPop(code, RBX);
        EmitAddRR(code, RAX, RBX);

        break;
    case ASSGN_S:
        EmitPop(code, RBX);
        EmitSubRR(code, RAX, RBX);

        break;
    case ASSGN_M:
        EmitPop(code, RBX);
        EmitImulRR(code, RAX, RBX);

        EmitMovRI(code, RBX, 10000);
        EmitCqo(code);
        EmitIdivR(code, RBX);

        break;
    case ASSGN_D:
        EmitMovRI(code,  RCX, 10000);
        EmitImulRR(code, RAX, RCX);

        EmitPop(code, RBX);
        EmitCqo(code);
        EmitIdivR(code, RBX);

        break;
    default:
        break;
    }

    if (node->value.ival != ASSGN)
        EmitMovMR(code, RBP, RAX, offset_tmp);
}

//--------------------------------------------------------------------

#define PRINT_CMP(cond) EmitPop(code, RBX);        \
                        EmitPop(code, RAX);        \
                        EmitCmpRR(code, RAX, RBX); \
                        EmitSet(code, cond, RAX);  \
                        EmitMovzx(code, RAX, RAX); \
                        EmitPush(code, RAX);

void FunctionCompiler::compile_expression(Node *node)
{
    assert(node != nullptr);

    switch (node->type)
    {
    case NUM:
        
        EmitMovRI(code, RAX, (int64_t)(node->value.dval * 10000));
        EmitPush(code, RAX);
        
        break;
    case IDENT:
        
        EmitMovRM(code, RAX, RBP, get_offset(node->value.strval));
        EmitPush(code, RAX);
        
        break;
    case BIN_OPER:

        compile_expression(node->lnode);
        compile_expression(node->rnode);
        
        switch (node->value.ival)
        {
        case ADD:
            EmitPop(code, RBX);
            EmitPop(code, RAX);
            EmitAddRR(code, RAX, RBX);
            EmitPush(code, RAX);
            
            break;
        case SUB:
            EmitPop(code, RBX);
            EmitPop(code, RAX);
            EmitSubRR(code, RAX, RBX);
            EmitPush(code, RAX);

            break;
        case MUL:
            EmitPop(code, RBX);
            EmitPop(code, RAX);
            EmitImulRR(code, RAX, RBX);
            
            EmitMovRI(code, RBX, 10000);
            EmitCqo(code);
            EmitIdivR(code, RBX);
            
            EmitPush(code, RAX);            
            
            break;
        case DIV:
            EmitPop(code,    RBX);
            EmitPop(code,    RAX);
            EmitMovRI(code,  RCX, 10000);
            EmitImulRR(code, RAX, RCX);

            EmitCqo(code);
            EmitIdivR(code, RBX);
            
            EmitPush(code, RAX);

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
            EmitPop(code, RBX);
            EmitPop(code, RAX);
            EmitOr(code, RAX, RBX);
            EmitPush(code, RAX);

            break;
        case AND:
            EmitPop(code, RBX);
            EmitPop(code, RAX);
            EmitAnd(code, RAX, RBX);
            EmitPush(code, RAX);

            break;
        default:
            printf("Unknown binary operation\n");
            break;
        }
        break;
    case INPUT:

        EmitCall(code, func_calls, "input");
        EmitPush(code, RAX);

        break;
    case FUN_CALL:
    {
        int arg_count = 0;
        if (node->lnode != nullptr)
            arg_count = compile_call_args(node->lnode);
        
        EmitCall(code, func_calls, node->value.strval);
        EmitAddRI(code, RSP, arg_count * 8);
        EmitPush(code, RAX);

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
