#pragma once

#include "node.h"

/* Binary operators */
typedef enum BinaryOp
{
    ADD    ,   // Addition                  (+)
    SUB    ,   // Subtraction               (-)
    MUL    ,   // Multiplication            (*)
    DIV    ,   // Division                  (/)
    LESS   ,   // Less                      (<)
    GREAT  ,   // Greater                   (>)
    LEQ    ,   // Less or equal             (<=)
    GEQ    ,   // Great or equal            (>=)
    EQ     ,   // Equal                     (==)
    NEQ    ,   // Not equal                 (!=)
    OR     ,   // Logic or                  (||)
    AND    ,   // Logic and                 (&&)
    ASSGN  ,   // Direct         assignment (= )
    ASSGN_ADD, // Addition       assignment (+=)
    ASSGN_SUB, // Subtraction    assignment (-=)
    ASSGN_MUL, // Multiplication assignment (*=)
    ASSGN_DIV  // Division       assignment (/=)
} BinaryOp;

/* Key words for lexical analysis */
typedef enum KeyWord
{
    ADD_KEY       = ADD,
    SUB_KEY       = SUB,
    MUL_KEY       = MUL,
    DIV_KEY       = DIV,
    LESS_KEY      = LESS, 
    GREAT_KEY     = GREAT,
    LEQ_KEY       = LEQ,
    GEQ_KEY       = GEQ,
    EQ_KEY        = EQ, 
    NEQ_KEY       = NEQ,
    OR_KEY        = OR, 
    AND_KEY       = AND, 
    ASSGN_KEY     = ASSGN,
    ASSGN_ADD_KEY = ASSGN_ADD,
    ASSGN_SUB_KEY = ASSGN_SUB,
    ASSGN_MUL_KEY = ASSGN_MUL,
    ASSGN_DIV_KEY = ASSGN_DIV,
    EOL_KEY,      // End of line 
    LBRACK_KEY,   // Left bracket 
    RBRACK_KEY,   // Right bracket 
    DECL_VAR_KEY, // Var declaration
    DECL_FUN_KEY, // Function declaration
    LBRACE_KEY,   // Left brace 
    RBRACE_KEY,   // Right brace
    COMMA_KEY,    
    IF_KEY,       // Begin of condition statement
    ELSE_KEY,     // If expression is false in condition statement
    LOOP_KEY,     // Loop with condition
    RET_KEY,      // Returning value in function
    INPUT_KEY,    // Input number
    PRINT_KEY     // Output number
} KeyWord;

typedef struct LexicalRule
{ 
    KeyWord keyword_type;
    NodeType node_type;
    /* If key_word is word, than it can't be stand with other symbols (without space) */
    char is_word;
    /* Key word */
    const char* word;
} LexicalRule;

const LexicalRule keywords_list[] =
{   
    {LEQ_KEY,       BIN_OPER, 0, "<=",                 },
    {GEQ_KEY,       BIN_OPER, 0, ">=",                 },
    {EQ_KEY,        BIN_OPER, 0, "==",                 },
    {NEQ_KEY,       BIN_OPER, 0, "!=",                 },
    {OR_KEY,        BIN_OPER, 0, "||",                 },
    {AND_KEY,       BIN_OPER, 0, "&&",                 },
    {ASSGN_ADD_KEY, ASSGN_ST, 1, "Add",                },
    {ASSGN_SUB_KEY, ASSGN_ST, 1, "Sub",                },
    {ASSGN_MUL_KEY, ASSGN_ST, 1, "Mul",                },
    {ASSGN_DIV_KEY, ASSGN_ST, 1, "Separate",           },
    {ASSGN_KEY  ,   ASSGN_ST, 1, "Become",             },
    {ADD_KEY,       BIN_OPER, 1, "Plus",               },
    {SUB_KEY,       BIN_OPER, 1, "Minus",              },
    {MUL_KEY,       BIN_OPER, 1, "Times",              },
    {DIV_KEY,       BIN_OPER, 1, "Divide",             },
    {LESS_KEY,      BIN_OPER, 1, "Less",               },
    {GREAT_KEY,     BIN_OPER, 1, "More",               },
    {EOL_KEY,       TRASH,    1, "The end",            },
    {LBRACK_KEY,    TRASH,    0, "(",                  },
    {RBRACK_KEY,    TRASH,    0, ")",                  },
    {DECL_VAR_KEY,  VAR_DECL, 1, "Beautiful friend",   },
    {DECL_FUN_KEY,  FUN_DECL, 1, "C'mon baby",         },
    {LBRACE_KEY,    TRASH,    0, "{",                  },
    {RBRACE_KEY,    TRASH,    0, "}",                  },
    {COMMA_KEY,     TRASH,    1, "and",                },
    {IF_KEY,        COND_ST,  1, "Break on through"    },
    {ELSE_KEY,      TRASH,    1, "To the other side"   },
    {LOOP_KEY,      WHILE_ST, 1, "When you're"         },
    {RET_KEY,       RET_ST,   1, "Blue bus is calling" },
    {INPUT_KEY,     INPUT,    1, "Your"                },
    {PRINT_KEY,     PRINT,    1, "I tell you"          },
};

/*struct Dumping_rule 
{
    Node_type type;
    const char* name;
};*/

/*const Dumping_rule names_types[] = 
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
};*/
