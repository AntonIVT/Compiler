#pragma once

#include <stdlib.h>

#define VECTOR_DEFAULT_SIZE 10

/*--------------------------------------------------------------------*/

/* */
typedef struct Vector
{
    size_t el_size;
    size_t capacity;
    size_t size;
    void *data;
} Vector;

/*====================================================================*/

/* */
void VectorConstruct(Vector* vector, size_t new_el_size);

/* */
void VectorDestruct(Vector* vector);

/* */
size_t VectorGetSize(Vector* vector);

/* */
void* VectorGet(Vector* vector, size_t index);

/* */
void VectorPushBack(Vector* vector, void* new_el);

/* */
void VectorPopBack(Vector* vector);
