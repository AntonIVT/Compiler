#pragma once

#include "tree.h"
#include "vector.h"
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring> 

// FuncCompaund  := Function {Function}*
// Function      := DECL_FUN Identifier FuncDeclArgs LCOMPBRACK ExpStatement {ExpStatement}* RCOMPBRACK
// FuncDeclArgs  := LBRACK [Identifier]? {COMMA Identifier}* RBRACK
// FuncCall      := Identifier LBRACK [Expression]? {COMMA Expression}* RBRACK
// ExpStatement  := [Assign | VarDecl | Print | RetStatement | FuncCall] END | [CondStatement | LoopStatement]
// CondStatement := COND  LBRACK EXPRESSION RBRACK [LCOMPBRACK]? ExpStatement {ExpStatement}* [RCOMPBRACK]? [ELSE [LCOMPBRACK]? ExpStatement {ExpStatement}* [RCOMPBRACK]?]?
// LoopStatement := WHILE LBRACK EXPRESSION RBRACK [LCOMPBRACK]? ExpStatement {ExpStatement}* [RCOMPBRACK]?
// RetStatement  := RET Expression
// Print         := PRINT Expression
// VarDecl       := DECL_VAR Identifier  [ASSGN Expression]?
// Assign        := Identifier [ASSGN, ASSGN_A, ASSGN_S, ASSGN_M, ASSGN_D] Expression
// Expression    := LogOper{[AND, OR] LogOper}* 
// LogOper       := CmpOper{[LESS, GREAT, LEQ, GEQ, EQ, NEQ] CmpOper}*    // logic operand
// CmpOper       := Term{[ADD, SUB] Term}*                                // compare operand
// Term          := Unary{[MUL, DIV] Unary}*
// Unary         := [ADD, SUB]? Priority
// Priority      := LBRACK Expression RBRACK | Number | Identifier | FuncCall | INPUT 
// Identifier    := Character {Character | '0-9'}*
// Character     := ['a-z' | 'A-Z' | '_']
// Number        := ['0-9']+ ? ['.'['0-9']+] ?

typedef struct Syntaxer
{
    Vector* lexemes;
    /* pointer to the current lexem in vector */
    int ptr;
};




struct Parser
{
private:
    Lexer lexer;
    List_Iterator ptr = 0;
    
    Node curr_node()
    {
        return lexer.list_nodes.get(ptr);
    }
    
    int curr_col()
    {
        return curr_node().column;
    }
    
    int curr_line()
    {
        return curr_node().line;
    }
    
    void next_node()
    {
        ptr = lexer.list_nodes.iter_inc(ptr);
    }
    
    void prev_node()
    {
        ptr = lexer.list_nodes.iter_dec(ptr);
    }
    
    bool isBinary()
    {
        if (curr_node().type == BIN_OPER)
            return true;
        else
            return false;
    }
    
    bool require(Key_words word)
    {
        if (curr_node().type == TRASH && curr_node().value.ival == word)
        {
            next_node();
            return true;
        }
        else
        {
            printf("Syntax error expected code:<%i>\n", word);
            printf("[%i:%i]\n", curr_node().line, curr_node().column);
            return false;
        }
    }
    
    #define REQUIRE(word)                   \
    if (!require(word))                     \
        {                                   \
            new_node->rec_destruct();       \
            return nullptr;                 \
        }
    
public:
    void construct(char *expression)
    {
        lexer.construct(expression);
        lexer.Lex_analys();
        ptr = lexer.list_nodes.begin();
    }
    
    void destruct()
    {
        lexer.destruct();
    }
    
    Node* get_Number()
    {
        if (curr_node().type == NUM)
        {
            Node* new_node = Node::get_new(curr_node());
            next_node();
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_Identifier()
    {           
        if (curr_node().type == IDENT)
        {            
            Node* new_node = Node::get_new(curr_node());
            next_node();
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_Priority()
    {
        Node *new_node = nullptr;
        
        if (curr_node().type == TRASH && curr_node().value.ival == LBRACK_K)
        {    
            next_node();
            if ((new_node = get_Expression()) == nullptr)
            {
                prev_node();
                return nullptr;
            }
            
            REQUIRE(RBRACK_K)
            return new_node;
        }
        if ((new_node = get_Number())     != nullptr)
            return new_node;
        if ((new_node = get_Input())      != nullptr)
            return new_node;
        if ((new_node = get_FuncCall())   != nullptr)
            return new_node;
        if ((new_node = get_Identifier()) != nullptr)
            return new_node;
            
        return nullptr;
    }
    
    Node* get_Input()
    {
        if (curr_node().type == INPUT)
        {
            Node* new_node = Node::get_new(curr_node());
            next_node();
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_Unary()
    {
        Node* new_node = nullptr;
                
        if (isBinary())
        {
            if (curr_node().value.ival == SUB)
            {
                next_node();
                new_node        = Node::get_new(Node::make(BIN_OPER, Node_val::make(MUL), curr_line(), curr_col()));
                new_node->lnode = Node::get_new(Node::make(NUM,      Node_val::make(-1.), curr_line(), curr_col()));
                new_node->rnode = get_Priority();
                
                if (new_node->rnode == nullptr)
                {
                    new_node->rec_destruct();
                    return nullptr;
                }
            
                return new_node;
            }
            else if (curr_node().value.ival == ADD)
                next_node();
            else
                return nullptr;
        }
        
        return get_Priority();
    }
    
    Node* get_Term()
    {
        Node *new_node_left = get_Unary();
        if (new_node_left == nullptr) return nullptr;
        
        if (isBinary() && (curr_node().value.ival == MUL || curr_node().value.ival == DIV))
        {
            Node *new_node = Node::get_new(curr_node());
            new_node->lnode = new_node_left;
            next_node();
            new_node->rnode = get_Term();
            
            if (new_node->rnode == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            return new_node;
        }
        else
            return new_node_left;
    }
    
    Node* get_CmpOper()
    {
        Node *new_node_left = get_Term();
        if (new_node_left == nullptr) return nullptr;
        
        if (isBinary() && (curr_node().value.ival == ADD || curr_node().value.ival == SUB))
        {
            Node *new_node = Node::get_new(curr_node());
            new_node->lnode = new_node_left;
            next_node();
            new_node->rnode = get_CmpOper();
            
            if (new_node->rnode == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            return new_node;
        }
        else
            return new_node_left;
    }
    
    Node* get_LogOper()
    {
        Node *new_node_left = get_CmpOper();
        if (new_node_left == nullptr) return nullptr;
        
        if (isBinary() && (curr_node().value.ival == LESS || curr_node().value.ival == GREAT ||
                           curr_node().value.ival == LEQ  || curr_node().value.ival == GEQ   ||
                           curr_node().value.ival == EQ   || curr_node().value.ival == NEQ))
        {
            Node *new_node = Node::get_new(curr_node());
            new_node->lnode = new_node_left;
            next_node();
            new_node->rnode = get_LogOper();
            
            if (new_node->rnode == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            return new_node;
        }
        else
            return new_node_left;
    }
    
    Node* get_Expression()
    {   
        Node *new_node_left = get_LogOper();
        if (new_node_left == nullptr) return nullptr;
        
        if (isBinary() && (curr_node().value.ival == AND || curr_node().value.ival == OR))
        {
            Node *new_node = Node::get_new(curr_node());
            new_node->lnode = new_node_left;
            next_node();
            new_node->rnode = get_Expression();
            
            if (new_node->rnode == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            return new_node;
        }
        else
            return new_node_left;
    }
    
    Node *get_Assign()
    {   
        if (curr_node().type == IDENT)
        {
            Node *new_node_left = Node::get_new(curr_node());
            next_node();
            
            if (curr_node().type == ASSGN_ST)
            {
                Node* new_node = Node::get_new(curr_node());
                new_node->lnode = new_node_left;
                next_node();
                
                new_node->rnode = get_Expression();
                if (new_node->rnode == nullptr)
                {
                    new_node->rec_destruct();
                    return nullptr;
                }
                
                return new_node;
            }
            else
            {
                new_node_left->rec_destruct();
                prev_node();
                return nullptr;
            }
        }
        else
            return nullptr;
    }
    
    Node* get_VarDecl()
    {
        if (curr_node().type == VAR_DECL)
        {
            next_node();
            if (curr_node().type == IDENT)
            {
                Node* new_node = Node::get_new(curr_node());
                new_node->type = VAR_DECL;
                next_node();
                
                if (curr_node().type == ASSGN_ST && curr_node().value.ival == ASSGN)
                {            
                    next_node();
                                        
                    new_node->lnode = get_Expression();
                    assert(new_node != nullptr);
                    if (new_node->lnode == nullptr) 
                    {
                        new_node->rec_destruct();
                        return nullptr;
                    }
                    else
                        return new_node;
                }
                else
                    return new_node;
            }
            else
                return nullptr;
        }
        else
            return nullptr;
    }
    
    Node* get_Print()
    {
        if(curr_node().type == PRINT)
        {
            Node* new_node = Node::get_new(curr_node());
            next_node();
            
            new_node->lnode = get_Expression();
            if (new_node->lnode == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_RetStatement()
    {
        if (curr_node().type == RET_ST)
        {
            Node *new_node = Node::get_new(curr_node());
            next_node();
            
            new_node->lnode = get_Expression();
            if (new_node->lnode == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_LoopStatement()
    {
        if (curr_node().type == WHILE_ST)
        {
            Node* new_node = Node::get_new(curr_node());
            next_node();
            
            REQUIRE(LBRACK_K)
            
            if ((new_node->lnode = get_Expression()) == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            REQUIRE(RBRACK_K)
            
            if ((new_node->rnode = get_ExpStmtComp()) == nullptr)
            {
                new_node->rnode = Node::get_new(Node::make(LINK, Node_val::make(0)));
                if ((new_node->rnode->lnode = get_ExpStatement()) == nullptr)
                {
                    new_node->rec_destruct();
                    return nullptr;   
                }
            }
            
            return new_node;
        }
        else
            return nullptr;
    }

    Node* get_ExpStatement()
    {
        Node* new_node = nullptr;
        
        if      ((new_node = get_FuncCall())      != nullptr) {}
        else if ((new_node = get_Assign())        != nullptr) {}
        else if ((new_node = get_VarDecl())       != nullptr) {}
        else if ((new_node = get_Print())         != nullptr) {}
        else if ((new_node = get_RetStatement())  != nullptr) {}
        else
        {
            if      ((new_node = get_CondStatement()) != nullptr) {}
            else if ((new_node = get_LoopStatement()) != nullptr) {}
            else
                return nullptr;
                
            return new_node;
        }       
        
        REQUIRE(END_K);
        
        return new_node;
    }
    
    Node* get_CondStatement()
    {
        if (curr_node().type == COND_ST)
        {
            Node* new_node = Node::get_new(curr_node());
            next_node();
            
            REQUIRE(LBRACK_K)
            
            if ((new_node->lnode = get_Expression()) == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
            
            REQUIRE(RBRACK_K)
            
            new_node->rnode = Node::get_new(Node::make(LINK, Node_val::make(0)));
            
            if (((new_node->rnode)->lnode = get_ExpStmtComp()) == nullptr)
            {
                new_node->rnode->lnode = Node::get_new(Node::make(LINK, Node_val::make(0)));
                if ((((new_node->rnode)->lnode)->lnode = get_ExpStatement()) == nullptr)
                {
                    new_node->rec_destruct();
                    return nullptr;   
                }
            }
            
            if (curr_node().type == TRASH && curr_node().value.ival == ELSE_K)
            {
                next_node();
                if (((new_node->rnode)->rnode = get_ExpStmtComp()) == nullptr)
                {
                    new_node->rnode->rnode = Node::get_new(Node::make(LINK, Node_val::make(0)));
                    if (((new_node->rnode->rnode)->lnode = get_ExpStatement()) == nullptr)
                    {
                        new_node->rec_destruct();
                        return nullptr;   
                    }
                }
            }
            else
                new_node->rnode->rnode = nullptr;
            
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_ExpStmtComp()
    {
        if (curr_node().type == TRASH && curr_node().value.ival == LCOMPBRACK_K)
        {
            next_node();
            Node *new_node = get_ExpStmtComp_rec();
            
            REQUIRE(RCOMPBRACK_K)
            
            return new_node;
        }
        else 
            return nullptr;
    }
    
    Node* get_ExpStmtComp_rec()
    {
        Node* new_node = Node::get_new(Node::make(LINK, Node_val::make(0)));
        
        if ((new_node->lnode = get_ExpStatement()) == nullptr)
        {
            new_node->rec_destruct();
            return nullptr;
        }
        
        new_node->rnode = get_ExpStmtComp_rec();
        
        return new_node;
    }
    
    Node* get_ArgsCall()
    {
        if (curr_node().type == TRASH && curr_node().value.ival == LBRACK_K)
        {
            next_node();
            Node* new_node = get_ArgsCall_rec();
            
            REQUIRE(RBRACK_K)
            
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_ArgsCall_rec()
    {
        Node* new_node = Node::get_new(Node::make(LINK, Node_val::make(0)));
        
        new_node->lnode = get_Expression();
        
        if (new_node->lnode == nullptr)
        {
            new_node->rec_destruct();
            return nullptr;
        }
        
        if (curr_node().type == TRASH && curr_node().value.ival == COMMA_K)
        {
            next_node();
            new_node->rnode = get_ArgsCall_rec();
        }
        
        return new_node;
    }
    
    Node* get_ArgsDecl()
    {
        if (curr_node().type == TRASH && curr_node().value.ival == LBRACK_K)
        {
            next_node();
            Node* new_node = get_ArgsDecl_rec();
            
            REQUIRE(RBRACK_K)
            
            return new_node;
        }
        else
            return nullptr;
    }
    
    Node* get_ArgsDecl_rec()
    {
        Node* new_node = Node::get_new(Node::make(LINK, Node_val::make(0)));
        
        new_node->lnode = get_Identifier();
        
        if (new_node->lnode == nullptr)
        {
            new_node->rec_destruct();
            return nullptr;
        }
        
        if (curr_node().type == TRASH && curr_node().value.ival == COMMA_K)
        {
            next_node();
            new_node->rnode = get_ArgsCall_rec();
        }
        
        return new_node;
    }
    
    Node* get_FuncCall()
    {
        if (curr_node().type == IDENT)
        {
            List_Iterator tmp = lexer.list_nodes.iter_inc(ptr);
            if (lexer.list_nodes.get(tmp).type != TRASH || lexer.list_nodes.get(tmp).value.ival != LBRACK_K)
                return nullptr;
            
            Node* new_node = Node::get_new(curr_node());
            new_node->type = FUN_CALL;
            next_node();                
        
            new_node->lnode = get_ArgsCall();
            
            return new_node;
        }
        else
            return nullptr;
    }
        
    Node* get_Function()
    {
        if (curr_node().type == FUN_DECL)
        {   
            next_node();
            
            if (curr_node().type != IDENT) return nullptr;
            
            Node* new_node = Node::get_new(curr_node());
            new_node->type = FUN_DECL;            
            
            next_node();
            new_node->lnode = get_ArgsDecl();
            
            if ((new_node->rnode = get_ExpStmtComp()) == nullptr)
            {
                new_node->rec_destruct();
                return nullptr;
            }
                    
            return new_node;
            
        }
        else
            return nullptr;
    }
    
    Node* get_FuncCompaund()
    {
        if (ptr == -1) return nullptr;
        if (curr_node().type == END_OF_FILE)
            return nullptr;
        
        Node* new_node = Node::get_new(Node::make(LINK, Node_val::make(0)));
        
        if ((new_node->lnode = get_Function()) == nullptr)
        {
            new_node->rec_destruct();
            printf("Syntax error! [%i:%i]\n", curr_line(), curr_col());
            return nullptr;
        }
        
        new_node->rnode = get_FuncCompaund();
        
        if (curr_node().type == END_OF_FILE)
            return new_node;
        else
        {
            printf("Syntax error!\n");  
            return nullptr;
        }  
    }
};
    #undef REQUIRE
    
    