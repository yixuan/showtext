#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include "outline.h"
#include "cairodev.h"

SEXP showtext_begin(SEXP dev_data);
SEXP showtext_end(void);

static R_CallMethodDef call_methods[] = {
    {"showtext_new_outline_funs",    (DL_FUNC) &showtext_new_outline_funs,    0},
    {"showtext_cairo_device_bitmap", (DL_FUNC) &showtext_cairo_device_bitmap, 0},
    {"showtext_begin_c",             (DL_FUNC) &showtext_begin,               1},
    {"showtext_end_c",               (DL_FUNC) &showtext_end,                 0},
    {NULL, NULL, 0}
};

void R_init_showtext(DllInfo* info)
{
    R_registerRoutines(info, NULL, call_methods, NULL, NULL);
    R_useDynamicSymbols(info, FALSE);
}
