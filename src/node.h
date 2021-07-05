#pragma once

/*  Types of nodes in AST-tree 
    Information:
    (Type of a value) $ (Type of the left node) $ (Type of the right node)
*/
typedef enum NodeType
{ 
    NUM         ,   // Float number                  $ NULL                              $ NULL
    IDENT       ,   // String                        $ NULL                              $ NULL
    BIN_OPER    ,   // Integer number (except assgn) $ NUM|IDENT|INPUT|BIN_OPER          $ NUM|IDENT|BIN_OPER|INPUT
    VAR_DECL    ,   // String                        $ NUM, IDENT, BIN_OPER, INPUT, NULL $ NULL
    FUN_DECL    ,   // String                        $ Link                              $ Link // слева - линк аргументов, справа - линк statements
    FUN_CALL    ,   // String                        $ Link                              $ NULL
    LINK        ,   // Non                           $ any                               $ NULL, Link
    INPUT       ,   // Non                           $ NULL                              $ NULL
    PRINT       ,   // Non                           $ NUM, IDENT, BIN_OPER, INPUT       $ NULL 
    COND_ST     ,   // Non                           $ NUM, IDENT, BIN_OPER, INPUT       $ LINK[LINK, LINK]
    WHILE_ST    ,   // Non                           $ NUM, IDENT, BIN_OPER, INPUT       $ LINK
    ASSGN_ST    ,   // Integer number (assgn)        $ IDENT                             $ NUM, IDENT, BIN_OPER, INPUT
    RET_ST      ,   // Non                           $ NUM, IDENT, BIN_OPER, INPUT       $ NULL
    TRASH       ,   // Brackets, end of expression and other things from lexer
    END_OF_FILE ,
} NodeType;

//--------------------------------------------------------------------

typedef union NodeVal
{
    double dval;
    int    ival;
    const char* strval;
} NodeVal;

//--------------------------------------------------------------------

NodeVal MakeNodeDval(double dval);
    
NodeVal MakeNodeIval(int ival);
    
NodeVal MakeNodeStrval(const char* strval);

//--------------------------------------------------------------------

typedef struct Node
{
    NodeType type;
    NodeVal value;
    
    int column;
    int line;
    
    struct Node* rnode;
    struct Node* lnode;
} Node;

//--------------------------------------------------------------------

/* Allocate memory */
Node* NodeConstruct(Node src_node);

Node NodeMake(NodeType type, NodeVal value, int line, int column);

void NodeDestruct(Node* node);

void NodeRecDestruct(Node* node);
