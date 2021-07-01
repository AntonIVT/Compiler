#include <assert.h>
#include <stddef.h>
#include "node.h"

//--------------------------------------------------------------------

NodeVal MakeNodeDval(double dval)
{
    NodeVal node_val = {0};
    node_val.dval = dval;
    
    return node_val;
}
    
NodeVal MakeNodeIval(int ival)
{
    NodeVal node_val = {0};
    node_val.ival = ival;
    
    return node_val;
}
    
NodeVal MakeNodeStrval(const char* strval)
{
    assert(strval != NULL);

    NodeVal node_val = {0};
    node_val.strval = strval;
    
    return node_val;
}

//--------------------------------------------------------------------

Node* NodeConstruct(NodeType type, NodeVal value, int line, int column)
{
    Node *new_node = (Node *)calloc(1, sizeof(Node));
    
    *new_node = NodeMake(type, value, line, column);
    
    return new_node;
}

Node NodeMake(NodeType type, NodeVal value, int line, int column)
{
    Node node = {type, value, line, column, NULL, NULL};    
}

void NodeDestruct(Node* node)
{
    assert(node != NULL);

    free(node);
}

void NodeRecDestruct(Node* node)
{
    if (node->lnode != NULL)
        NodeRecDestruct(node->lnode);
    if (node->rnode != NULL)
        NodeRecDestruct(node->rnode);
    
    if (node->type == IDENT || node->type == FUN_CALL || node->type == FUN_DECL || node->type == VAR_DECL)
        free(node->value.strval);
    
    free(node);
}
