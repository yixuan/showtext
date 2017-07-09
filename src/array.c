#include "array.h"

Array Array_new(int initial_size)
{
    Array array = (Array) malloc(sizeof(struct Array_s));
    if(!array)
    {
        Rf_error("failed to allocate memory");
        return array; /* should not get here */
    }
    array->len       = 0;
    array->alloc_len = initial_size;
    array->data      = (double*) calloc(array->alloc_len, sizeof(double));
    if(!array->data)
    {
        Rf_error("failed to allocate memory");
        return array; /* should not get here */
    }

    return array;
}

void Array_append(Array array, double val)
{
    if(!array) return;
    if(array->len + 1 > array->alloc_len)
    {
        array->alloc_len *= 2;
        array->data = (double*) realloc(array->data, array->alloc_len * sizeof(double));
    }
    array->data[array->len] = val;
    array->len++;
}

void Array_destroy(Array array)
{
    if(!array) return;
    if(array->data) free(array->data);
    if(array)       free(array);
}



IntArray IntArray_new(int initial_size)
{
    IntArray array = (IntArray) malloc(sizeof(struct IntArray_s));
    if(!array)
    {
        Rf_error("failed to allocate memory");
        return array; /* should not get here */
    }
    array->len       = 0;
    array->alloc_len = initial_size;
    array->data      = (int*) calloc(array->alloc_len, sizeof(int));
    if(!array->data)
    {
        Rf_error("failed to allocate memory");
        return array; /* should not get here */
    }
    
    return array;
}

void IntArray_append(IntArray array, int val)
{
    if(!array) return;
    if(array->len + 1 > array->alloc_len)
    {
        array->alloc_len *= 2;
        array->data = (int*) realloc(array->data, array->alloc_len * sizeof(int));
    }
    array->data[array->len] = val;
    array->len++;
}

void IntArray_destroy(IntArray array)
{
    if(!array) return;
    if(array->data) free(array->data);
    if(array)       free(array);
}
