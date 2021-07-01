#pragma once 

#include "vector.h"
#include "node.h"

/* Lexic analyzer, splits program code into lexemes (in vector_nodes) */

typedef struct Lexer
{
    char *ptr;  // current pointer in file
    int column; 
    int line;   
    Vector* lexemes;
} Lexer;

void LexerSkip(Lexer* lexer);
    
void LexerConstruct(Lexer* lexer, char *expression);

void LexerDestruct(Lexer* lexer);

void LexerIdentDestruct(Lexer* lexer);

char* LexerGetIdent(Lexer* lexer);

double LexerGetNum(Lexer* lexer);

/* if returning 0 => success, else => smth wrong */
char LexerAnalyze(Lexer* lexer);
