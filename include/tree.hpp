#pragma once

#include <cstdlib>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>

enum Binary_op
{
    ADD   = 1,  // +
    SUB   = 2,  // -
    MUL   = 3,  // *
    DIV   = 4,  // /
    LESS  = 5,  // <
    GREAT = 6,  // >
    LEQ   = 7,  // <=
    GEQ   = 8,  // >=
    EQ    = 9, // ==
    NEQ   = 10, // !=
    OR    = 11, // ||
    AND   = 12  // &&
};

enum Assgn_op
{
    ASSGN   = 14, // eq   =
    ASSGN_A = 15, // add +=
    ASSGN_S = 16, // sub -=
    ASSGN_M = 17, // mul *=
    ASSGN_D = 18  // div /=
};

enum Key_words
{
    ADD_K     = ADD,
    SUB_K     = SUB,
    MUL_K     = MUL,
    DIV_K     = DIV,
    LESS_K    = LESS, 
    GREAT_K   = GREAT,
    LEQ_K     = LEQ,
    GEQ_K     = GEQ,
    EQ_K      = EQ, 
    NEQ_K     = NEQ,
    OR_K      = OR, 
    AND_K     = AND, 
    ASSGN_K   = ASSGN,
    ASSGN_A_K = ASSGN_A,
    ASSGN_S_K = ASSGN_S,
    ASSGN_M_K = ASSGN_M,
    ASSGN_D_K = ASSGN_D,
    END_K,
    LBRACK_K,
    RBRACK_K,
    DECL_VAR_K,
    DECL_FUN_K,
    LCOMPBRACK_K,
    RCOMPBRACK_K,
    COMMA_K,
    COND_K,
    ELSE_K,
    WHILE_K,
    RET_K,
    INPUT_K,
    PRINT_K
};

enum Node_type 
{ 
    NUM         = 1,   // num $ NULL $ NULL
    IDENT       = 2,   // str $ NULL $ NULL
    BIN_OPER    = 3,   // "+, ^, >=, ||" $ NUM, IDENT, INPUT, BIN_OPER $ NUM, IDENT, BIN_OPER, INPUT, NULL
    VAR_DECL    = 4,   // str $ NUM, IDENT, BIN_OPER, INPUT, NULL $ NULL
    FUN_DECL    = 5,   // str $ Link $ Link // слева - линк аргументов, справа - линк statements
    FUN_CALL    = 6,   // str $ Link $ NULL
    LINK        = 7,   // no  $ any  $ NULL, Link
    INPUT       = 8,   // no  $ NULL $ NULL
    PRINT       = 9,   // no  $ NUM, IDENT, BIN_OPER, INPUT $ NULL 
    COND_ST     = 10,  // no  $ NUM, IDENT, BIN_OPER, INPUT $ LINK[LINK, LINK]
    WHILE_ST    = 11,  // no  $ NUM, IDENT, BIN_OPER, INPUT $ LINK
    ASSGN_ST    = 12,  // "=, +=, /=", $ IDENT $ NUM, IDENT, BIN_OPER, INPUT
    RET_ST      = 13,  // no  $ NUM, IDENT, BIN_OPER, INPUT $ NULL
    TRASH       = 100, // Brackets, end of expression and other things from lexer
    END_OF_FILE = 101
};

struct Parsing_rule
{
    const char *word;
    Key_words num;
    Node_type type;
    bool isWord;
};

struct Dumping_rule
{
    Node_type type;
    const char* name;
};

union Node_val
{
    double dval;
    int ival;
    char *strval;
    
    static Node_val make(double dval)
    {
        Node_val tmp = {};
        tmp.dval = dval;
        return tmp;
    }
    
    static Node_val make(int ival)
    {
        Node_val tmp = {};
        tmp.ival = ival;
        return tmp;
    }
    
    static Node_val make(char* strval)
    {
        Node_val tmp = {};
        tmp.strval = strval;
        return tmp;
    }
};

//const Parsing_rule words_list[] =
//{   
//    {"<=",                       LEQ_K,        BIN_OPER, false},
//    {">=",                       GEQ_K,        BIN_OPER, false},
//    {"==",                       EQ_K,         BIN_OPER, false},
//    {"!=",                       NEQ_K,        BIN_OPER, false},
//    {"||",                       OR_K,         BIN_OPER, false},
//    {"&&",                       AND_K,        BIN_OPER, false},
//    {"+=",                       ASSGN_A_K,    ASSGN_ST, false},
//    {"-=",                       ASSGN_S_K,    ASSGN_ST, false},
//    {"*=",                       ASSGN_M_K,    ASSGN_ST, false},
//    {"/=",                       ASSGN_D_K,    ASSGN_ST, false},
//    {"=",                        ASSGN_K  ,    ASSGN_ST, true},
//    {"+",                        ADD_K,        BIN_OPER, false},
//    {"-",                        SUB_K,        BIN_OPER, false},
//    {"*",                        MUL_K,        BIN_OPER, false},
//    {"/",                        DIV_K,        BIN_OPER, false},
//    {"<",                       LESS_K,       BIN_OPER, true},
//    {">",                       GREAT_K,      BIN_OPER, true},
//    {";",                        END_K,        TRASH,    true},
//    {"(",                        LBRACK_K,     TRASH,    false},
//    {")",                        RBRACK_K,     TRASH,    false},
//    {"var",         DECL_VAR_K,   VAR_DECL, true},
//    {"func",          DECL_FUN_K,   FUN_DECL, true},
//    {"{",                        LCOMPBRACK_K, TRASH,    false},
//    {"}",                        RCOMPBRACK_K, TRASH,    false},
//    {",",                      COMMA_K,      TRASH,    true},
//    {"if",                 COND_K,       COND_ST,  true},
//    {"else",              ELSE_K,       TRASH,    true},
//    {"while",           WHILE_K,      WHILE_ST, true},
//    {"return",      RET_K,        RET_ST,   true},
//    {"in",                       INPUT_K,      INPUT,    true},
//    {"out",                      PRINT_K,      PRINT,    true}
//};

const Parsing_rule words_list[] =
{   
    {"<=",                       LEQ_K,        BIN_OPER, false},
    {">=",                       GEQ_K,        BIN_OPER, false},
    {"==",                       EQ_K,         BIN_OPER, false},
    {"!=",                       NEQ_K,        BIN_OPER, false},
    {"||",                       OR_K,         BIN_OPER, false},
    {"&&",                       AND_K,        BIN_OPER, false},
    {"add",                       ASSGN_A_K,    ASSGN_ST, false},
    {"sub",                      ASSGN_S_K,    ASSGN_ST, false},
    {"mul",                      ASSGN_M_K,    ASSGN_ST, false},
    {"separate",                 ASSGN_D_K,    ASSGN_ST, false},
    {"become",                   ASSGN_K  ,    ASSGN_ST, true},
    {"plus",                     ADD_K,        BIN_OPER, false},
    {"minus",                    SUB_K,        BIN_OPER, false},
    {"times",                    MUL_K,        BIN_OPER, false},
    {"divide",                   DIV_K,        BIN_OPER, false},
    {"less",                     LESS_K,       BIN_OPER, true},
    {"more",                     GREAT_K,      BIN_OPER, true},
    {"the end",                  END_K,        TRASH,    true},
    {"(",                        LBRACK_K,     TRASH,    false},
    {")",                        RBRACK_K,     TRASH,    false},
    {"Beautiful friend",         DECL_VAR_K,   VAR_DECL, true},
    {"C'mon baby",               DECL_FUN_K,   FUN_DECL, true},
    {"{",                        LCOMPBRACK_K, TRASH,    false},
    {"}",                        RCOMPBRACK_K, TRASH,    false},
    {"and",                      COMMA_K,      TRASH,    true},
    {"Break on through",         COND_K,       COND_ST,  true},
    {"To the other side",        ELSE_K,       TRASH,    true},
    {"When you're",              WHILE_K,      WHILE_ST, true},
    {"Blue bus is calling",      RET_K,        RET_ST,   true},
    {"your",                     INPUT_K,      INPUT,    true},
    {"I tell you",               PRINT_K,      PRINT,    true}
};

const Dumping_rule names_types[] = 
{
    {NUM         , "Number"    },
    {IDENT       , "Identifier"},
    {BIN_OPER    , "Operator"  },
    {VAR_DECL    , "VarDef"  },
    {FUN_DECL    , "FuncDecl" },
    {FUN_CALL    , "FuncCall" },
    {LINK        , "Link"    }, 
    {INPUT       , "Input"     },
    {PRINT       , "Print"     },
    {COND_ST     , "IfStmt"        },
    {WHILE_ST    , "WhileStmt"     },
    {ASSGN_ST    , "AssignExpr"     },
    {RET_ST      , "ReturnStmt"    },
    {TRASH       , ""          },
    {END_OF_FILE , ""          }
};

struct Loading_rule_op
{
    const char *name;
    Binary_op oper;
};

const Loading_rule_op names_oper[] = 
{
    {"+", ADD},
    {"-", SUB},
    {"*", MUL},
    {"/", DIV},
    {"<", LESS},
    {">", GREAT},
    {"<=", LEQ},
    {">=", GEQ},
    {"==", EQ},
    {"!=", NEQ},
    {"||", OR},
    {"&&", AND}    
};

struct Loading_rule_assg
{
    const char *name;
    Assgn_op oper;
};

const Loading_rule_assg names_assgn[] =
{
    {"=", ASSGN},
    {"+=", ASSGN_A},
    {"-=", ASSGN_S},
    {"*=", ASSGN_M},
    {"/=", ASSGN_D}  
};

struct Node
{    
    Node_type type;
    Node_val value;
    int column;
    int line;
    Node* rnode;
    Node* lnode;
    
    static Node make(Node_type type, Node_val value, int line = 0, int column = 0)
    {
        Node tmp = {};
        tmp.type   = type;
        tmp.value  = value;
        tmp.line   = line;
        tmp.column = column;
        tmp.rnode  = nullptr;
        tmp.lnode  = nullptr;
        return tmp;
    }
    
    static Node* get_new(Node other)
    {
        Node *new_node = (Node *)calloc(1, sizeof(Node));
        
        new_node->type   = other.type;
        new_node->value  = other.value;
        new_node->column = other.column;
        new_node->line   = other.line;
        new_node->rnode  = nullptr;
        new_node->lnode  = nullptr;
        
        return new_node;
    }
    
    void rec_destruct()
    {
        if (lnode != nullptr)
            lnode->rec_destruct();
        if (rnode != nullptr)
            rnode->rec_destruct();
        
        if (type == IDENT || type == FUN_CALL || type == FUN_DECL || type == VAR_DECL)
            free(value.strval);
        
        free(this);
    }
    
    void Dump(FILE* dump)
    {
        const char * name = nullptr;
        for(int i = 0; i < sizeof(names_types) / sizeof(names_types[0]); i++)
        {
            if (type == names_types[i].type)
                name = names_types[i].name;
        }
        
        if (type == NUM)
        {
            fprintf(dump, "struct_%p [style = filled, fillcolor = \"#8ddfff\","
                          "label = \" %s<%lg>:%i:%i \" ];\n", this, name, value.dval, line, column);
        }
        
        else if (type == BIN_OPER || type == ASSGN_ST)
        {
            const char* node_name = nullptr;
            for (int i = 0; i < sizeof(words_list) / sizeof(words_list[0]); i++)
                if (words_list[i].num == value.ival) 
                    node_name = words_list[i].word;
            assert(node_name);
            
            fprintf(dump, "struct_%p [style = filled, fillcolor = \"#8ddfff\","
                          "label = \" %s<%s>:%i:%i \" ];\n", this, name, node_name, line, column);
        }
        
        else if (type == IDENT || type == FUN_CALL || type == FUN_DECL || type == VAR_DECL)
        {
            fprintf(dump, "struct_%p [style = filled, fillcolor = \"#8ddfff\","
                          "label = \" %s<%s>:%i:%i \" ];\n", this, name, value.strval, line, column);
        }
        
        else
        {
            fprintf(dump, "struct_%p [style = filled, fillcolor = \"#8ddfff\","
                          "label = \" %s<>:%i:%i \" ];\n", this, name, value.strval, line, column);
        }
            
        if (lnode != nullptr)
        {
            lnode->Dump(dump);
            fprintf(dump, "edge [color = \"#ff9d14\"];\n struct_%p->struct_%p [style = bold,label = \"L\"];\n", this, lnode);
        }
        if (rnode != nullptr)
        {
            rnode->Dump(dump);
            fprintf(dump, "edge [color = \"#ff5533\"];\n struct_%p->struct_%p [style = bold,label = \"R\"];\n", this, rnode);
        }
    }
    
};

struct Tree
{
    Node *root;    

    void construct(Node *root = nullptr)
    {
        this->root = root;
    }
    
    void destruct()
    {
        if (root)
            root->rec_destruct();
    }
    
    void Dump(const char *dump_file)
    {
        FILE* dump = fopen("Dump.gv", "wb");
        fprintf(dump, "digraph structs {\n");
        
        if (root == nullptr){
            fclose(dump);
            return;
        }
        
        root->Dump(dump);
        fprintf(dump, "}\n");
        fclose(dump);
        
        char syst[100] = "dot -Tpdf Dump.gv -o "; 
        strcat(syst, dump_file);
        strcat(syst, " && rm Dump.gv");
        
        system(syst);
    }
};
