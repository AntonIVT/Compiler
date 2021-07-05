#pragma once

#include "vector.h"
#include "node.h"

// Syntax:
// {} - optional 
// [] - obligatory
// {}*, []* - repeat ({}* could be 0 times)
// INPUT, ADD, ... - keywords (INPUT_KEY, ADD_KEY, ...)
//
// FuncCompaund  := [Function {Function}*]
// Function      := [DECL_FUN Identifier FuncDeclArgs LBRACE Statement {Statement}* RBRACE]
// FuncDeclArgs  := [LBRACK {Identifier {COMMA Identifier}*} RBRACK]
// Statement     := [[CondStatement | LoopStatement | [Assign | VarDecl | Print | RetStatement | FuncCall] EOL]]
// FuncCall      := [Identifier LBRACK {Expression {COMMA Expression}*} RBRACK]
// Cond          := [IF LBRACK EXPRESSION RBRACK {LBRACE} ExpStatement {ExpStatement}* {RBRACE} {ELSE {LBRACE} ExpStatement {ExpStatement}* {RBRACE}}]
// Loop          := [LOOP LBRACK EXPRESSION RBRACK {LBRACE} ExpStatement {ExpStatement}* {RBRACE}]
// Return        := [RET Expression]
// Print         := [PRINT Expression]
// VarDecl       := [DECL_VAR Identifier {ASSGN Expression}]
// Assign        := [Identifier [ASSGN | ASSGN_ADD | ASSGN_SUB | ASSGN_MUL | ASSGN_DIV] Expression]
// Expression    := [LogOper{[AND | OR] LogOper}*]
// LogicOper     := [CmpOper{[LESS | GREAT | LEQ | GEQ | EQ | NEQ] CmpOper}*]   // logic operand
// CompareOper   := [Term{[ADD | SUB] Term}*]                                   // compare operand
// Term          := [Unary{[MUL | DIV] Unary}*]
// Unary         := [{ADD | SUB} Priority]
// Priority      := [LBRACK Expression RBRACK | Number | Identifier | FuncCall | Input]
// Input         := [INPUT]
// Identifier    := [Character {Character | '0-9'}*]
// Character     := ['a-z' | 'A-Z' | '_']
// Number        := ['0-9']*  {'.'['0-9']{'0-9'}*}

typedef struct SyntaxAnalyzer
{
    Vector* lexemes;
    /* pointer to the current lexem in vector */
    int ptr;
} SyntaxAnalyzer;

void SyntaxConstruct(SyntaxAnalyzer* syntaxer, Vector* lexemes);
    
void SyntaxDestruct(SyntaxAnalyzer* syntaxer);

/* Ret - root of the AST tree, if NULL => smth wrong */
Node* SyntaxAnalyze(SyntaxAnalyzer* syntaxer);
