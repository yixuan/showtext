#include "array.h"

Array ArrayNew(int initialSize)
{
    Array array = (Array) malloc(sizeof(struct Array_s));
    if(!array)
    {
        Rf_error("fail to allocate memory");
        return array; /* should not get here */
    }
    array->len = 0;
    array->alloc_len = initialSize;
    array->data = (double *) calloc(array->alloc_len, sizeof(double));
    if(!array->data)
    {
        Rf_error("fail to allocate memory");
        return array; /* should not get here */
    }

    return array;
}

void ArrayAppend(Array array, double val)
{
    if(!array) return;
    if(array->len + 1 > array->alloc_len)
    {
        array->alloc_len *= 2;
        array->data = (double *) realloc(array->data, array->alloc_len * sizeof(double));
    }
    array->data[array->len] = val;
    array->len++;
}

void ArrayDestroy(Array array)
{
    if(!array) return;
    if(array->data) free(array->data);
    if(array) free(array);
}
