#include "syntax_analyzer.h"
#include "tree_rules.h"
#include <stdlib.h>

/* Helpful functions for recursive descent */
static Node SyntaxCurrNode(SyntaxAnalyzer* syntaxer);
    
static char SyntaxIsBinary(SyntaxAnalyzer* syntaxer);

static char SyntaxRequire(SyntaxAnalyzer* syntaxer, KeyWord keyword);

/* Recursive descent syntax analyzer */

static Node* SyntaxGetNum(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetIdentifier(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetPriority(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetInput(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetUnary(SyntaxAnalyzer* syntaxer);
    
static Node* SyntaxGetTerm(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetCompareOper(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetLogicOper(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetExpression(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetAssign(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetVarDecl(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetPrint(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetReturn(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetLoop(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetCond(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetFuncCall(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetStatement(SyntaxAnalyzer* syntaxer);
    
static Node* SyntaxGetFuncDeclArgs(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetFunction(SyntaxAnalyzer* syntaxer);

static Node* SyntaxGetFuncCompaund(SyntaxAnalyzer* syntaxer);


void SyntaxConstruct(SyntaxAnalyzer* syntaxer, Vector* lexemes)
{
    syntaxer->lexemes = lexemes;
    syntaxer->ptr = 0;
}

void SyntaxDestruct(SyntaxAnalyzer* syntaxer)
{
    syntaxer->ptr = 0;
    //VectorDestruct(syn)
}

static Node SyntaxCurrNode(SyntaxAnalyzer* syntaxer)
{
    return *(Node *)VectorGet(syntaxer->lexemes, syntaxer->ptr);
}

static char SyntaxIsBinary(SyntaxAnalyzer* syntaxer)
{
    return (SyntaxCurrNode(syntaxer).type == BIN_OPER ? 1 : 0);
}

static char SyntaxRequire(SyntaxAnalyzer* syntaxer, KeyWord keyword)
{
    if (SyntaxCurrNode(syntaxer).type == TRASH && 
        SyntaxCurrNode(syntaxer).value.ival == keyword)
    {
        syntaxer->ptr++;
        return 1;
    }
    else
    {
        printf("Syntax error expected code:<%i>\n", keyword);
        printf("[%i:%i]\n", SyntaxCurrNode(syntaxer).line,
                            SyntaxCurrNode(syntaxer).column);
        return 0;
    }
}

#define REQUIRE(keyword)                      \
    if (!SyntaxRequire(syntaxer, keyword))    \
    {                                         \
        NodeRecDestruct(new_node);            \
        return NULL;                          \
    }               

static Node* SyntaxGetNum(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == NUM)
        {
            Node* new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
            syntaxer->ptr++;
            return new_node;
        }
        else
            return NULL;
}

static Node *SyntaxGetIdentifier(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == IDENT)
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;
        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetPriority(SyntaxAnalyzer* syntaxer)
{
    Node *new_node = NULL;

    if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == LBRACK_KEY)
    {
        syntaxer->ptr++;
        if ((new_node = SyntaxGetExpression(syntaxer)) == NULL)
        {
            syntaxer->ptr--;
            return NULL;
        }

        REQUIRE(RBRACK_KEY)
        return new_node;
    }
    if ((new_node = SyntaxGetNumber(syntaxer)) != NULL)
        return new_node;
    if ((new_node = SyntaxGetInput(syntaxer)) != NULL)
        return new_node;
    if ((new_node = SyntaxGetFuncCall(syntaxer)) != NULL)
        return new_node;
    if ((new_node = SyntaxGetIdentifier(syntaxer)) != NULL)
        return new_node;

    return NULL;
}

static Node *SyntaxGetInput(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == INPUT)
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;
        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetUnary(SyntaxAnalyzer* syntaxer)
{
    Node *new_node = NULL;

    if (SyntaxIsBinary(syntaxer))
    {
        if (SyntaxCurrNode(syntaxer).value.ival == SUB)
        {
            syntaxer->ptr++;
            new_node = NodeConstruct(NodeMake(BIN_OPER,  MakeNodeIval(MUL), SyntaxCurrNode(syntaxer).line, SyntaxCurrNode(syntaxer).column));
            new_node->lnode = NodeConstruct(NodeMake(NUM,  MakeNodeDval(-1.), SyntaxCurrNode(syntaxer).line, SyntaxCurrNode(syntaxer).column));
            new_node->rnode = SyntaxGetPriority(syntaxer);

            if (new_node->rnode == NULL)
            {
                NodeRecDestruct(new_node);
                return NULL;
            }

            return new_node;
        }
        else if (SyntaxCurrNode(syntaxer).value.ival == ADD)
            syntaxer->ptr++;
        else
            return NULL;
    }

    return SyntaxGetPriority(syntaxer);
}

static Node *SyntaxGetTerm(SyntaxAnalyzer* syntaxer)
{
    Node *new_node_left = SyntaxGetUnary(syntaxer);
    if (new_node_left == NULL)
        return NULL;

    if (SyntaxIsBinary(syntaxer) && (SyntaxCurrNode(syntaxer).value.ival == MUL || SyntaxCurrNode(syntaxer).value.ival == DIV))
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        new_node->lnode = new_node_left;
        syntaxer->ptr++;
        new_node->rnode = SyntaxGetTerm(syntaxer);

        if (new_node->rnode == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return new_node_left;
}

static Node *SyntaxGetCmpOper(SyntaxAnalyzer* syntaxer)
{
    Node *new_node_left = SyntaxGetTerm(syntaxer);
    if (new_node_left == NULL)
        return NULL;

    if (SyntaxIsBinary(syntaxer) && (SyntaxCurrNode(syntaxer).value.ival == ADD || SyntaxCurrNode(syntaxer).value.ival == SUB))
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        new_node->lnode = new_node_left;
        syntaxer->ptr++;
        new_node->rnode = SyntaxGetCmpOper(syntaxer);

        if (new_node->rnode == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return new_node_left;
}

static Node *SyntaxGetLogOper(SyntaxAnalyzer* syntaxer)
{
    Node *new_node_left = SyntaxGetCmpOper(syntaxer);
    if (new_node_left == NULL)
        return NULL;

    if (SyntaxIsBinary(syntaxer) && (SyntaxCurrNode(syntaxer).value.ival == LESS || SyntaxCurrNode(syntaxer).value.ival == GREAT ||
                                     SyntaxCurrNode(syntaxer).value.ival == LEQ  || SyntaxCurrNode(syntaxer).value.ival == GEQ ||
                                     SyntaxCurrNode(syntaxer).value.ival == EQ   || SyntaxCurrNode(syntaxer).value.ival == NEQ))
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        new_node->lnode = new_node_left;
        syntaxer->ptr++;
        new_node->rnode = SyntaxGetLogOper(syntaxer);

        if (new_node->rnode == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return new_node_left;
}

static Node *SyntaxGetExpression(SyntaxAnalyzer* syntaxer)
{
    Node *new_node_left = SyntaxGetLogOper(syntaxer);
    if (new_node_left == NULL)
        return NULL;

    if (SyntaxIsBinary(syntaxer) && (SyntaxCurrNode(syntaxer).value.ival == AND || SyntaxCurrNode(syntaxer).value.ival == OR))
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        new_node->lnode = new_node_left;
        syntaxer->ptr++;
        new_node->rnode = SyntaxGetExpression(syntaxer);

        if (new_node->rnode == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return new_node_left;
}

Node *get_Assign(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == IDENT)
    {
        Node *new_node_left = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;

        if (SyntaxCurrNode(syntaxer).type == ASSGN_ST)
        {
            Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
            new_node->lnode = new_node_left;
            syntaxer->ptr++;

            new_node->rnode = SyntaxGetExpression(syntaxer);
            if (new_node->rnode == NULL)
            {
                NodeRecDestruct(new_node);
                return NULL;
            }

            return new_node;
        }
        else
        {
            NodeRecDestruct(new_node_left);
            syntaxer->ptr--;
            return NULL;
        }
    }
    else
        return NULL;
}

static Node *SyntaxGetVarDecl(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == VAR_DECL)
    {
        syntaxer->ptr++;
        if (SyntaxCurrNode(syntaxer).type == IDENT)
        {
            Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
            new_node->type = VAR_DECL;
            syntaxer->ptr++;

            if (SyntaxCurrNode(syntaxer).type == ASSGN_ST && SyntaxCurrNode(syntaxer).value.ival == ASSGN)
            {
                syntaxer->ptr++;

                new_node->lnode = SyntaxGetExpression(syntaxer);
                assert(new_node != NULL);
                if (new_node->lnode == NULL)
                {
                    NodeRecDestruct(new_node);
                    return NULL;
                }
                else
                    return new_node;
            }
            else
                return new_node;
        }
        else
            return NULL;
    }
    else
        return NULL;
}

static Node *SyntaxGetPrint(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == PRINT)
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;

        new_node->lnode = SyntaxGetExpression(syntaxer);
        if (new_node->lnode == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetRetStatement(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == RET_ST)
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;

        new_node->lnode = SyntaxGetExpression(syntaxer);
        if (new_node->lnode == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetLoopStatement(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == WHILE_ST)
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;

        REQUIRE(LBRACK_KEY)

        if ((new_node->lnode = SyntaxGetExpression(syntaxer)) == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        REQUIRE(RBRACK_KEY)

        if ((new_node->rnode = SyntaxGetExpStmtComp(syntaxer)) == NULL)
        {
            new_node->rnode = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));
            if ((new_node->rnode->lnode = SyntaxGetExpStatement(syntaxer)) == NULL)
            {
                NodeRecDestruct(new_node);
                return NULL;
            }
        }

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetExpStatement(SyntaxAnalyzer* syntaxer)
{
    Node *new_node = NULL;

    if ((new_node = SyntaxGetFuncCall(syntaxer)) != NULL)
    {}
    else if ((new_node = SyntaxGetAssign(syntaxer)) != NULL)
    {}
    else if ((new_node = SyntaxGetVarDecl(syntaxer)) != NULL)
    {}
    else if ((new_node = SyntaxGetPrint(syntaxer)) != NULL)
    {}
    else if ((new_node = SyntaxGetRetStatement(syntaxer)) != NULL)
    {}
    else
    {
        if ((new_node = SyntaxGetCondStatement(syntaxer)) != NULL)
        {}
        else if ((new_node = SyntaxGetLoopStatement(syntaxer)) != NULL)
        {}
        else
            return NULL;

        return new_node;
    }

    REQUIRE(EOL_KEY);

    return new_node;
}

static Node *SyntaxGetCondStatement(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == COND_ST)
    {
        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        syntaxer->ptr++;

        REQUIRE(LBRACK_KEY)

        if ((new_node->lnode = SyntaxGetExpression(syntaxer)) == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        REQUIRE(RBRACK_KEY)

        new_node->rnode = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));

        if (((new_node->rnode)->lnode = SyntaxGetExpStmtComp(syntaxer)) == NULL)
        {
            new_node->rnode->lnode = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));
            if ((((new_node->rnode)->lnode)->lnode = SyntaxGetExpStatement(syntaxer)) == NULL)
            {
                NodeRecDestruct(new_node);
                return NULL;
            }
        }

        if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == ELSE_KEY)
        {
            syntaxer->ptr++;
            if (((new_node->rnode)->rnode = SyntaxGetExpStmtComp(syntaxer)) == NULL)
            {
                new_node->rnode->rnode = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));
                if (((new_node->rnode->rnode)->lnode = SyntaxGetExpStatement(syntaxer)) == NULL)
                {
                    NodeRecDestruct(new_node);
                    return NULL;
                }
            }
        }
        else
            new_node->rnode->rnode = NULL;

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetExpStmtComp(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == LBRACE_KEY)
    {
        syntaxer->ptr++;
        Node *new_node = SyntaxGetExpStmtCompRec(syntaxer);

        REQUIRE(RBRACE_KEY)

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetExpStmtCompRec(syntaxer)
{
    Node *new_node = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));

    if ((new_node->lnode = SyntaxGetExpStatement(syntaxer)) == NULL)
    {
        NodeRecDestruct(new_node);
        return NULL;
    }

    new_node->rnode = SyntaxGetExpStmtCompRec();

    return new_node;
}

static Node *SyntaxGetArgsCall(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == LBRACK_KEY)
    {
        syntaxer->ptr++;
        Node *new_node = SyntaxGetArgsCallRec(syntaxer);

        REQUIRE(RBRACK_KEY)

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetArgsCallRec(SyntaxAnalyzer* syntaxer)
{
    Node *new_node = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));

    new_node->lnode = SyntaxGetExpression(syntaxer);

    if (new_node->lnode == NULL)
    {
        NodeRecDestruct(new_node);
        return NULL;
    }

    if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == COMMA_KEY)
    {
        syntaxer->ptr++;
        new_node->rnode = SyntaxGetArgsCallRec(syntaxer);
    }

    return new_node;
}

static Node *SyntaxGetArgsDecl(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == LBRACK_KEY)
    {
        syntaxer->ptr++;
        Node *new_node = SyntaxGetArgsDeclRec(syntaxer);

        REQUIRE(RBRACK_KEY)

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetArgsDeclRec(SyntaxAnalyzer* syntaxer)
{
    Node *new_node = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));

    new_node->lnode = SyntaxGetIdentifier(syntaxer);

    if (new_node->lnode == NULL)
    {
        NodeRecDestruct(new_node);
        return NULL;
    }

    if (SyntaxCurrNode(syntaxer).type == TRASH && SyntaxCurrNode(syntaxer).value.ival == COMMA_KEY)
    {
        syntaxer->ptr++;
        new_node->rnode = SyntaxGetArgsCallRec(syntaxer);
    }

    return new_node;
}

static Node *SyntaxGetFuncCall(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == IDENT)
    {
        if (((Node* )VectorGet(syntaxer->lexemes, syntaxer->ptr + 1))->type != TRASH || ((Node* )VectorGet(syntaxer->lexemes, syntaxer->ptr + 1))->value.ival != LBRACK_KEY)
            return NULL;

        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        new_node->type = FUN_CALL;
        syntaxer->ptr++;

        new_node->lnode = SyntaxGetArgsCall(syntaxer);

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetFunction(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == FUN_DECL)
    {
        syntaxer->ptr++;

        if (SyntaxCurrNode(syntaxer).type != IDENT)
            return NULL;

        Node *new_node = NodeConstruct(SyntaxCurrNode(syntaxer));
        new_node->type = FUN_DECL;

        syntaxer->ptr++;
        new_node->lnode = SyntaxGetArgsDecl(syntaxer);

        if ((new_node->rnode = SyntaxGetExpStmtComp(syntaxer)) == NULL)
        {
            NodeRecDestruct(new_node);
            return NULL;
        }

        return new_node;
    }
    else
        return NULL;
}

static Node *SyntaxGetFuncCompaund(SyntaxAnalyzer* syntaxer)
{
    if (SyntaxCurrNode(syntaxer).type == END_OF_FILE)
        return NULL;

    Node *new_node = NodeConstruct(NodeMake(LINK,  MakeNodeIval(0), -1, -1));

    if ((new_node->lnode = SyntaxGetFunction(syntaxer)) == NULL)
    {
        NodeRecDestruct(new_node);
        printf("Syntax error! [%i:%i]\n", SyntaxCurrNode(syntaxer).line, SyntaxCurrNode(syntaxer).column);
        return NULL;
    }

    new_node->rnode = SyntaxGetFuncCompaund(syntaxer);

    if (SyntaxCurrNode(syntaxer).type == END_OF_FILE)
        return new_node;
    else
    {
        printf("Syntax error!\n");
        return NULL;
    }
}

#undef REQUIRE
