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
    double *data;
    int len;
    int alloc_len;
};

typedef struct Array_s* Array;

Array ArrayNew(int initialSize);
void ArrayAppend(Array array, double val);
void ArrayDestroy(Array array);

#endif /* ARRAY_H_INCLUDED */
