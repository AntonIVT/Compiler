#pragma once

#include "node.h"

typedef struct Tree
{
    Node *root;  
} Tree;

void TreeConstruct(Tree *tree, Node *root);
    
void TreeDestruct(Tree* tree);
