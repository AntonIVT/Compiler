#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vector.h"


void VectorConstruct(Vector* vector, size_t new_el_size)
{
    assert(vector != NULL);

    vector->el_size = new_el_size; 
    vector->data    = calloc(VECTOR_DEFAULT_SIZE, new_el_size);
    vector->capacity = VECTOR_DEFAULT_SIZE;
    vector->size = 0;
}

void VectorDestruct(Vector* vector)
{
    assert(vector != NULL);
    assert(vector->data != NULL);

    free(vector->data);
}

static void VectorResize(Vector* vector)
{
    assert(vector != NULL);

    if (vector->size == vector->capacity)
    {
        void* new_data = realloc(vector->data, vector->el_size * vector->capacity * 2);
        assert(new_data != NULL);
        
        vector->data = new_data;
        vector->capacity *= 2;
    }
}

size_t VectorGetSize(Vector* vector)
{
    assert(vector != NULL);

    return vector->size;
}

void* VectorGet(Vector* vector, size_t index)
{
    assert(vector != NULL);
    assert(index < vector->size);
    
    return vector->data + index * vector->el_size;
}

void VectorPushBack(Vector* vector, void* new_el)
{
    assert(vector != NULL);
    assert(new_el != NULL);

    if (vector->capacity == vector->size)
        VectorResize(vector);

    memcpy(vector->data + vector->size * vector->el_size, new_el, vector->el_size);
    (vector->size)++;
}

void VectorPopBack(Vector* vector)
{
    assert(vector != NULL);

    (vector->size)--;
}

