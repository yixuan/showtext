#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include "outline.h"
#include "cairodev.h"

SEXP showtext_null_pointer();
SEXP showtext_new_dev_desc();
SEXP showtext_free_dev_desc(SEXP ext_ptr);
SEXP showtext_begin();
SEXP showtext_end();

static R_CallMethodDef call_methods[] = {
    {"showtext_new_outline_funs",    (DL_FUNC) &showtext_new_outline_funs,    0},
    {"showtext_free_outline_funs",   (DL_FUNC) &showtext_free_outline_funs,   1},
    {"showtext_null_pointer",        (DL_FUNC) &showtext_null_pointer,        0},
    {"showtext_new_dev_desc",        (DL_FUNC) &showtext_new_dev_desc,        0},
    {"showtext_free_dev_desc",       (DL_FUNC) &showtext_free_dev_desc,       1},
    {"showtext_cairo_device_bitmap", (DL_FUNC) &showtext_cairo_device_bitmap, 0},
    {"showtext_begin",               (DL_FUNC) &showtext_begin,               0},
    {"showtext_end",                 (DL_FUNC) &showtext_end,                 0},
    {NULL, NULL, 0}
};

void R_init_showtext(DllInfo* info)
{
    R_registerRoutines(info, NULL, call_methods, NULL, NULL);
    R_useDynamicSymbols(info, FALSE);
}
