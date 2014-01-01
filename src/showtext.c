#include "util.h"

void showtextMetricInfo(int c, const pGEcontext gc, double* ascent, double* descent, double* width, pDevDesc dd);
double showtextStrWidthUTF8(const char *str, const pGEcontext gc, pDevDesc dd);
void showtextTextUTF8(double x, double y, const char *str, double rot, double hadj, const pGEcontext gc, pDevDesc dd);

SEXP showtextReturnNullPointer()
{
    SEXP extPtr;

    extPtr = R_MakeExternalPtr(NULL, R_NilValue, R_NilValue);
    return extPtr;
}

SEXP showtextLoadDevDesc()
{
    pDevDesc dd_save = (pDevDesc) calloc(1, sizeof(DevDesc));
    SEXP extPtr;

    extPtr = R_MakeExternalPtr(dd_save, R_NilValue, R_NilValue);
    return extPtr;
}

SEXP showtextCleanDevDesc(SEXP extPtr)
{
    pDevDesc dd_save = (pDevDesc) R_ExternalPtrAddr(extPtr);
    if(dd_save) free(dd_save);
    
    return R_NilValue;
}

SEXP showtextBegin()
{
    int currDev = curDevice();
    SEXP extPtr;
    pGEDevDesc gdd;
    pDevDesc dd;
    
    if(currDev == 0)
        Rf_error("no active graphics device");
        
    gdd = GEgetDevice(currDev);
    dd = gdd->dev;
    
    /* Save the current gdd */
    extPtr = R_MakeExternalPtr(gdd, R_NilValue, R_NilValue);
    Rf_setVar(install(".gdd.save"), extPtr, GetPkgEnv("showtext"));
    /* Save the current dd */
    *(GetDevDesc()) = *dd;
    
    /* Replace the text functions */
    dd->canHAdj = TRUE;
    dd->metricInfo = showtextMetricInfo;
    dd->hasTextUTF8 = TRUE;
    dd->text = showtextTextUTF8;
    dd->textUTF8 = showtextTextUTF8;
    dd->strWidth = showtextStrWidthUTF8;
    dd->strWidthUTF8 = showtextStrWidthUTF8;
    dd->wantSymbolUTF8 = TRUE;

    return R_NilValue;
}

SEXP showtextEnd()
{
    int currDev = curDevice();
    pGEDevDesc gdd;
    pDevDesc dd;
    
    if(currDev == 0)
        Rf_error("no active graphics device");
        
    gdd = GEgetDevice(currDev);
    dd = gdd->dev;
    
    if(gdd != GetGEDevDesc())
    {
        Rf_error("current device does not match the one that uses showtext.begin()");
    }
    
    /* Restore dd */
    *dd = *(GetDevDesc());

    return R_NilValue;
}

