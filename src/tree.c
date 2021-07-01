#include <assert.h>
#include <stddef.h>
#include "tree.h"

void TreeConstruct(Tree* tree, Node *root)
{
    assert(tree != NULL);

    tree->root = root;
}

void TreeDestruct(Tree* tree)
{
    assert(tree != NULL);

    if (tree->root != NULL)
        NodeRecDestruct(tree->root);
}

