#include "util.h"


SEXP GetPkgEnv(const char *pkgName)
{
    SEXP pkgNS, pkgEnv;
    PROTECT(pkgNS = R_FindNamespace(ScalarString(mkChar(pkgName))));
    PROTECT(pkgEnv = Rf_findVar(install(".pkg.env"), pkgNS));
    if(TYPEOF(pkgEnv) == PROMSXP) {
        PROTECT(pkgEnv);
        pkgEnv = eval(pkgEnv, pkgNS);
        UNPROTECT(1);
    }
    UNPROTECT(2);
    
    return pkgEnv;
}

SEXP GetVarFromPkgEnv(const char *varName, const char *pkgName)
{
    /* See grDevices/src/devPS getFontDB() */
    SEXP pkgNS, pkgEnv, var;
    PROTECT(pkgNS = R_FindNamespace(ScalarString(mkChar(pkgName))));
    PROTECT(pkgEnv = Rf_findVar(install(".pkg.env"), pkgNS));
    if(TYPEOF(pkgEnv) == PROMSXP) {
        PROTECT(pkgEnv);
        pkgEnv = eval(pkgEnv, pkgNS);
        UNPROTECT(1);
    }
    PROTECT(var = Rf_findVar(install(varName), pkgEnv));
    UNPROTECT(3);
    
    return var;
}

FT_Outline_Funcs* GetFTOutlineFuncs()
{
    FT_Outline_Funcs *funs;
    SEXP extPtr = GetVarFromPkgEnv(".outline_funs", "showtext");
    funs = (FT_Outline_Funcs *) R_ExternalPtrAddr(extPtr);

    return funs;
}

int GetNseg()
{
    SEXP nseg = GetVarFromPkgEnv(".nseg", "showtext");

    return INTEGER(nseg)[0];
}

double GetDevUnitsPerPoint()
{
    SEXP dev_units_per_point = GetVarFromPkgEnv(".dev_units_per_point", "showtext");

    return REAL(dev_units_per_point)[0];
}

pDevDesc GetSavedDevDesc()
{
    pDevDesc dd_save;
    SEXP extPtr = GetVarFromPkgEnv(".dd_saved", "showtext");
    dd_save = (pDevDesc) R_ExternalPtrAddr(extPtr);

    return dd_save;
}

pGEDevDesc GetSavedDeviceID()
{
    pGEDevDesc gdd_saved;
    SEXP extPtr = GetVarFromPkgEnv(".device_id", "showtext");
    gdd_saved = (pGEDevDesc) R_ExternalPtrAddr(extPtr);

    return gdd_saved;
}

Rboolean UseRaster()
{
    SEXP use_raster = GetVarFromPkgEnv(".use_raster", "showtext");

    return LOGICAL(use_raster)[0];
}
