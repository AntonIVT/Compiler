#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lexical_analyzer.h"
#include "tree_rules.h"
#include "node.h"

static void LexicSkip(LexicalAnalyzer* lexer);

static char* LexicGetIdent(LexicalAnalyzer* lexer);

static double LexicGetNum(LexicalAnalyzer* lexer);

static void LexicSkip(LexicalAnalyzer* lexer)
{
    while (isspace(*(lexer->ptr)))
    {
        if (*(lexer->ptr) == '\n')
        {
            lexer->column = 1;
            lexer->line++;
        }
        else
            lexer->column++;
        lexer->ptr++;
    }
}
    
void LexicConstruct(LexicalAnalyzer* lexer, char *expression)
{
    assert(lexer != NULL);
    assert(expression != NULL);

    lexer->ptr    = expression;
    lexer->column = 1;
    lexer->line   = 1;

    lexer->lexemes = (Vector*)calloc(1, sizeof(Vector));
    VectorConstruct(lexer->lexemes, sizeof(Node));
}

void LexicDestruct(LexicalAnalyzer* lexer)
{        
    assert(lexer != NULL);

    VectorDestruct(lexer->lexemes);
    free(lexer->lexemes);
}

void LexicIdentDestruct(LexicalAnalyzer* lexer)
{
    assert(lexer != NULL);

    int size = VectorGetSize(lexer->lexemes);

    for (int i = 0; i < size; i++)
    {
        Node* curr_el = (Node*)VectorGet(lexer->lexemes, i);
        
        if (curr_el->type == IDENT)
            free((void*)curr_el->value.strval);
    }    
}

static char* LexicGetIdent(LexicalAnalyzer* lexer)
{
    assert(lexer != NULL);

    int ident_len = 0;
    while(*(lexer->ptr + ident_len) == '_' || isalpha(*(lexer->ptr + ident_len)) || isdigit(*(lexer->ptr + ident_len)))
    {
        lexer->column++;
        ident_len++;
    }
    
    /* Remember last symbol */
    char last_symb = *(lexer->ptr + ident_len);
    *(lexer->ptr + ident_len) = '\0';
    
    char* ident = strdup(lexer->ptr);
    lexer->ptr = lexer->ptr + ident_len;
    *lexer->ptr = last_symb;
    
    return ident;
}

static double LexicGetNum(LexicalAnalyzer* lexer)
{
    assert(lexer != NULL);

    char* num_end = NULL;
    char* num_begin = lexer->ptr;
    double num = strtod(num_begin, &num_end);
    
    lexer->column += (num_end - num_begin);
    lexer->ptr = num_end;
    
    return num;
}

char LexicAnalyze(LexicalAnalyzer* lexer)
{
    assert(lexer != NULL);

    char is_wrong = 0;

    while(*(lexer->ptr) != '\0')
    {
        LexicSkip(lexer);
        
        if (isdigit(*lexer->ptr))
        {   
            Node new_node = NodeMake(NUM, MakeNodeDval(LexicGetNum(lexer)), lexer->line, lexer->column);
            VectorPushBack(lexer->lexemes,  &new_node);   
            continue;
        }
          
        char is_keyword = 0;
        
        for (int i = 0; i < sizeof(keywords_list) / sizeof(keywords_list[0]); i++)
        {
            
            if (lexer->ptr == strstr(lexer->ptr, keywords_list[i].word))
            {   
                int len = strlen(keywords_list[i].word);
                if ((isdigit(*(lexer->ptr + len)) || isalpha(*(lexer->ptr + len)) || *(lexer->ptr + len) == '_') && keywords_list[i].is_word)
                    continue;
                
                Node new_node = NodeMake(keywords_list[i].node_type, MakeNodeIval(keywords_list[i].keyword_type), lexer->line, lexer->column);
                VectorPushBack(lexer->lexemes, &new_node);
                
                lexer->column += len;                
                lexer->ptr += len;
        
                is_keyword = 1;
                break;
            }
        }
        
        if (is_keyword == 1)
            continue;
        
        if (*lexer->ptr == '_' || isalpha(*lexer->ptr))
        {
            Node new_node = NodeMake(IDENT, MakeNodeStrval(LexicGetIdent(lexer)), lexer->line, lexer->column);
            VectorPushBack(lexer->lexemes, &new_node);
            continue;
        }

        if (!isspace(*lexer->ptr) && *lexer->ptr != '\0')
        {
            printf("Wrong lexical:\n <%20s>\n [%i:%i]\n", lexer->ptr, lexer->line, lexer->column);
            is_wrong = 1;
            LexicIdentDestruct(lexer);

            break;
        }
    }

    Node last_node = NodeMake(END_OF_FILE, MakeNodeIval(1), 0, 0);
    VectorPushBack(lexer->lexemes, &last_node);

    return is_wrong;
}
