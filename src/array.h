/*
  Resizeable arrays.
  Mainly used to record the outline points of characters.
*/

#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

struct Array_s {
    double* data;
    int     len;
    int     alloc_len;
};
typedef struct Array_s* Array;

Array Array_new(int initial_size);
void  Array_append(Array array, double val);
void  Array_destroy(Array array);


/* Integer version */
struct IntArray_s {
    int* data;
    int  len;
    int  alloc_len;
};
typedef struct IntArray_s* IntArray;

IntArray IntArray_new(int initial_size);
void     IntArray_append(IntArray array, int val);
void     IntArray_destroy(IntArray array);

#endif /* ARRAY_H_INCLUDED */
