#pragma once

#include "tree.hpp"
#include "list.hpp"
#include "struct.hpp"

//--------------------------------------------------------------------

struct FunctionTranslator
{
    List<Variable> name_table;
    FILE* file;
    Node* func_decl;
    int curr_offset;
    bool isReturn = false;
    
    void construct(Node* func_decl, FILE* file);
    
    void destruct();
    
    int  get_offset(const char *Variable);
        
    void translate_func();
    
    void translate_func_args(Node* node, int curr_offset);
    
    void translate_exp_stmt_comp(Node *node);
    
    void translate_cond(Node *node);
    
    void translate_while(Node* node);
    
    void translate_ret(Node *node);
    
    void translate_print(Node *node);
    
    void translate_var_decl(Node* node);
    
    void translate_assgn(Node *node);

    void translate_expression(Node *node);
    
    int  translate_call_args(Node* node);
};

//--------------------------------------------------------------------

struct ProgramTranslator
{
    FILE* file;
    Tree *tree;
    
    void construct(const char* file_name, Tree *tree);
    
    void destruct();
    
    void translate_program();
    
    void translate_func(Node* node);
};

#define PRINT_FILE(...) \
    fprintf(file, __VA_ARGS__);

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// ProgramTranslator methods
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void ProgramTranslator::construct(const char *file_name, Tree *tree)
{
    assert(file_name != nullptr);
    assert(tree != nullptr);

    file = fopen(file_name, "wb");
    assert(file != nullptr);

    this->tree = tree;
}

//--------------------------------------------------------------------

void ProgramTranslator::destruct()
{
    if (file != nullptr)
        fclose(file);
}

//--------------------------------------------------------------------

void ProgramTranslator::translate_program()
{
    assert(file != nullptr);

    PRINT_FILE( "global _start\n"
                "extern output\n"
                "section .text\n\n");

    if (tree->root != nullptr)
        translate_func(tree->root);
}

//--------------------------------------------------------------------

void ProgramTranslator::translate_func(Node *node)
{
    if (node == nullptr || node->type != LINK)
        return;

    if (node->lnode != nullptr)
    {
        FunctionTranslator func_translator = {};
        func_translator.construct(node->lnode, file);
        func_translator.translate_func();
        func_translator.destruct();
    }

    if (node->rnode != nullptr)
        translate_func(node->rnode);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// FunctionTranslator methods
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void FunctionTranslator::construct(Node *func_decl, FILE *file)
{
    assert(func_decl != nullptr && file != nullptr);

    this->func_decl = func_decl;
    this->file = file;
    name_table.construct(1);
    curr_offset = 0;
}

//--------------------------------------------------------------------

void FunctionTranslator::destruct()
{
    name_table.destruct();
}

//--------------------------------------------------------------------

int FunctionTranslator::get_offset(const char *Variable)
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
    /* TO DO add error*/
    abort();
}

//--------------------------------------------------------------------

#define PRINT_INFO //fprintf(file, "\n\t; %s: [%i:%i]\n", __func__, node->line, node->column);

void FunctionTranslator::translate_func()
{
    assert(func_decl != nullptr && func_decl->type == FUN_DECL);

    if (!strcmp(func_decl->value.strval, MAIN_FUNC))
    {
        PRINT_FILE( "db \"HERE START LOL\"\n"
                    "_start:\n"
                    "\tmov rbp, rsp\n");
    }
    else
    {
        PRINT_FILE( "%s:\n"
                    "\tpush rbp\n"
                    "\tmov rbp, rsp\n",
                    func_decl->value.strval);
    }

    if (func_decl->lnode != nullptr)
        translate_func_args(func_decl->lnode, 16);

    translate_exp_stmt_comp(func_decl->rnode);

    if (!strcmp(func_decl->value.strval, MAIN_FUNC))
    {
        PRINT_FILE( "\tadd rsp, %i\n"
                    "\tmov rax, 60\n"
                    "\tmov rdi, 0\n"
                    "\tsyscall\n",
                    -curr_offset);
    }
    else
        PRINT_FILE( "\tadd rsp, %i\n"
                    "\tpop rbp\n"
                    "\tmov rax, 0\n"
                    "\tret\n",
                    -curr_offset);
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_func_args(Node *node, int curr_offset)
{
    assert(node != nullptr && node->type == LINK);

    if (node->lnode != nullptr)
        name_table.push_back(Variable::Create(node->lnode->value.strval, curr_offset));

    if (node->rnode != nullptr)
        translate_func_args(node->rnode, curr_offset + 8);
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_exp_stmt_comp(Node *node)
{
    assert(node != nullptr && node->type == LINK);

    PRINT_INFO

    if (node->lnode != nullptr)
    {
        switch (node->lnode->type)
        {
        case ASSGN_ST:
            translate_assgn(node->lnode);
            break;
        case RET_ST:
            translate_ret(node->lnode);
            break;
        case VAR_DECL:
            translate_var_decl(node->lnode);
            break;
        case PRINT:
            translate_print(node->lnode);
            break;
        case WHILE_ST:
            translate_while(node->lnode);
            break;
        case COND_ST:
            translate_cond(node->lnode);
            break;
        case FUN_CALL:
        {
            int arg_count = 0;
            if (node->lnode != nullptr)
                arg_count = translate_call_args(node->lnode);
            PRINT_FILE( "\tcall %s\n"
                        "\tadd rsp, %i\n",
                        node->value.strval, arg_count * 8);
            break;
        }
        default:
            break;
        }
    }
    if (node->rnode != nullptr)
        translate_exp_stmt_comp(node->rnode);
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_cond(Node *node)
{
    assert(node != nullptr && node->lnode != nullptr && node->rnode != nullptr &&
           node->rnode->lnode != nullptr);

    PRINT_INFO

    translate_expression(node->lnode);

    PRINT_FILE( "\tmov rax, [rsp]\n"
                "\tadd rsp, 8\n"
                "\tcmp rax, 0\n"
                "\tje false_cond_%i_%i\n",
                node->line, node->column);
    
    translate_exp_stmt_comp(node->rnode->lnode);

    PRINT_FILE( "\tjmp true_cond_%i_%i\n"
                "\tfalse_cond_%i_%i:\n",
                node->line, node->column, node->line, node->column);
    
    if (node->rnode->rnode != nullptr)
        translate_exp_stmt_comp(node->rnode->rnode);

    PRINT_FILE("\ttrue_cond_%i_%i:\n", node->line, node->column);
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_while(Node *node)
{
    assert(node != nullptr && node->lnode != nullptr && node->rnode != nullptr);

    PRINT_INFO

    PRINT_FILE("\twhile_begin_%i_%i:\n", node->line, node->column);
    
    translate_expression(node->lnode);
    
    PRINT_FILE( "\tmov rax, [rsp]\n"
                "\tadd rsp, 8\n"
                "\tcmp rax, 0\n"
                "\tje while_end_%i_%i\n",
                node->line, node->column);
    
    translate_exp_stmt_comp(node->rnode);

    PRINT_FILE( "\tjmp while_begin_%i_%i\n"
                "\twhile_end_%i_%i:\n",
                node->line, node->column, node->line, node->column);
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_ret(Node *node)
{
    assert(node != nullptr);

    translate_expression(node->lnode);
    
    PRINT_INFO
    PRINT_FILE( "\tpop rax\n"
                "\tadd rsp, %i\n"
                "\tpop rbp\n"
                "\tret\n",
                -curr_offset);

    isReturn = true;
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_print(Node *node)
{
    assert(node != nullptr);

    translate_expression(node->lnode);
    
    PRINT_INFO
    PRINT_FILE( "\tcall output\n"
                "\tadd rsp, 8\n");
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_var_decl(Node *node)
{
    assert(node != nullptr);

    PRINT_INFO

    name_table.push_back(Variable::Create(node->value.strval, curr_offset - 8));

    if (node->lnode != nullptr)
        translate_expression(node->lnode);
    else
        PRINT_FILE("\tpush QWORD %i\n", rand());

    curr_offset -= 8;
}

//--------------------------------------------------------------------

void FunctionTranslator::translate_assgn(Node *node)
{
    assert(node != nullptr && node->lnode != nullptr && node->lnode->type == IDENT);

    PRINT_INFO

    int offset_tmp = get_offset(node->lnode->value.strval);

    translate_expression(node->rnode);

    if (node->value.ival != ASSGN)
        PRINT_FILE("\tmov rax, [rbp+%i]\n", offset_tmp);

    switch (node->value.ival)
    {
    case ASSGN:
        PRINT_FILE("\tpop QWORD [rbp+%i]\n", offset_tmp);
        break;
    case ASSGN_A:
        PRINT_FILE("\tadd rax, [rsp]\n", offset_tmp);
        break;
    case ASSGN_S:
        PRINT_FILE("\tsub rax, [rsp]\n", offset_tmp);
        break;
    case ASSGN_M:
        PRINT_FILE("\timul rax, [rsp]\n", offset_tmp);
        break;
    case ASSGN_D:
        PRINT_FILE( "\tmov rbx, [rsp]\n"
                    "\tmov rdx, 0\n"
                    "\tidiv rbx\n",
                    offset_tmp);
        break;
    default:
        break;
    }

    if (node->value.ival != ASSGN)
        PRINT_FILE( "\tmov [rbp+%i], rax\n"
                    "\tadd rsp, 8\n",
                    offset_tmp);
}

//--------------------------------------------------------------------

#define PRINT_OPER(cond) fprintf(file,  "\tmov rax, [rsp + 8]\n"    \
                                        "\t" #cond " rax, [rsp]\n"  \
                                        "\tmov [rsp + 8], rax\n"    \
                                        "\tadd rsp, 8\n");

#define PRINT_CMP(cond) fprintf(file,   "\tmov rax, [rsp + 8]\n"    \
                                        "\tcmp rax, [rsp]\n"        \
                                        "\tset" #cond " al\n"       \
                                        "\tmovzx rax, al\n"         \
                                        "\tmov [rsp + 8], rax\n"    \
                                        "\tadd rsp, 8\n");

void FunctionTranslator::translate_expression(Node *node)
{
    assert(node != nullptr);

    switch (node->type)
    {
    case NUM:
        PRINT_INFO
        PRINT_FILE( "\tmov rax, %lli\n"
                    "\tpush rax\n",
                    (long long)node->value.dval);
        break;
    case IDENT:
        PRINT_INFO
        fprintf(file, "\tpush QWORD [rbp+%i]\n", get_offset(node->value.strval));
        break;
    case BIN_OPER:
        translate_expression(node->lnode);
        translate_expression(node->rnode);
        PRINT_INFO
        switch (node->value.ival)
        {
        case ADD:
            PRINT_OPER(add)
            break;
        case SUB:
            PRINT_OPER(sub)
            break;
        case MUL:
            PRINT_OPER(imul)
            break;
        case DIV:
            PRINT_FILE( "\tmov rdx, 0\n"
                        "\tmov rax, [rsp + 8]\n"
                        "\tidiv QWORD [rsp]\n"
                        "\tmov [rsp + 8], rax\n"
                        "\tadd rsp, 8\n")
            break;
        case LESS:
            PRINT_CMP(l)
            break;
        case GREAT:
            PRINT_CMP(g)
            break;
        case LEQ:
            PRINT_CMP(le)
            break;
        case GEQ:
            PRINT_CMP(ge)
            break;
        case EQ:
            PRINT_CMP(e)
            break;
        case NEQ:
            PRINT_CMP(ne)
            break;
        case OR:
            PRINT_FILE( "\tcmp QWORD [rsp + 8], 0\n"
                        "\tjne true_%p\n"
                        "\tcmp QWORD [rsp], 0\n"
                        "\tjne true_%p\n"
                        "\tmov QWORD [rsp + 8], 0\n"
                        "\tjmp end_%p\n"
                        "\ttrue_%p:\n"
                        "\tmov QWORD [rsp + 8], 1\n"
                        "\tend_%p:\n"
                        "\tadd rsp, 8\n",
                        node, node, node, node, node);
            break;
        case AND:
            PRINT_FILE( "\tcmp QWORD [rsp + 8], 0\n"
                        "\tje  false_%p\n"
                        "\tcmp QWORD [rsp], 0\n"
                        "\tje  false_%p\n"
                        "\tmov QWORD [rsp + 8], 1\n"
                        "\tjmp end_%p\n"
                        "\tfalse_%p:\n"
                        "\tmov QWORD [rsp + 8], 0\n"
                        "\tend_%p:\n"
                        "\tadd rsp, 8\n",
                        node, node, node, node, node);
            break;
        default:
            printf("Unknown binary operation\n");
            break;
        }
        break;
    case INPUT:
        PRINT_INFO
        PRINT_FILE( "\tcall input\n"
                    "\tpush rax\n");
        break;
    case FUN_CALL:
    {
        PRINT_INFO
        int arg_count = 0;
        if (node->lnode != nullptr)
            arg_count = translate_call_args(node->lnode);
        
        PRINT_FILE( "\tcall %s\n" 
                    "\tadd rsp, %i\n"
                    "\tpush rax\n",
                    node->value.strval, arg_count * 8);

        break;
    }
    default:
        break;
    }
}

#undef PRINT_OPER
#undef PRINT_CMP

//--------------------------------------------------------------------

int FunctionTranslator::translate_call_args(Node *node)
{
    assert(node != nullptr && node->type == LINK);

    int arg_count = 0;

    if (node->rnode != nullptr)
        arg_count = translate_call_args(node->rnode);

    if (node->lnode != nullptr)
        translate_expression(node->lnode);

    return arg_count + 1;
}

//--------------------------------------------------------------------

#undef PRINT_FILE
#undef PRINT_INFO
