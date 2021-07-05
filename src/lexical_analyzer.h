#pragma once 

#include "vector.h"
/* Lexic analyzer, splits program code into lexemes (in vector_nodes) */

typedef struct LexicalAnalyzer
{
    char *ptr;  // current pointer in file
    int column; 
    int line;   
    Vector* lexemes;
} LexicalAnalyzer;
    
void LexicConstruct(LexicalAnalyzer* lexer, char *expression);

void LexicDestruct(LexicalAnalyzer* lexer);

void LexicIdentDestruct(LexicalAnalyzer* lexer);

/* if returning 0 => success, else => smth wrong */
char LexicAnalyze(LexicalAnalyzer* lexer);
