#include "util.h"


SEXP get_pkg_env(const char* pkg_name)
{
    /* See grDevices/src/devPS getFontDB() */
    SEXP pkg_namespace, pkg_env;
    PROTECT(pkg_namespace = R_FindNamespace(Rf_ScalarString(Rf_mkChar(pkg_name))));
    PROTECT(pkg_env = Rf_findVar(Rf_install(".pkg.env"), pkg_namespace));
    if(TYPEOF(pkg_env) == PROMSXP) {
        pkg_env = Rf_eval(pkg_env, pkg_namespace);
        /* Unprotect the old pkg_env */
        UNPROTECT(1);
        /* Protect the new pkg_env */
        PROTECT(pkg_env);
    }
    UNPROTECT(2);
    
    return pkg_env;
}

SEXP get_var_from_pkg_env(const char* var_name, const char* pkg_name)
{
    SEXP pkg_env, var;
    
    pkg_env = PROTECT(get_pkg_env(pkg_name));
    var = PROTECT(Rf_findVar(Rf_install(var_name), pkg_env));
    UNPROTECT(2);
    
    return var;
}

FT_Outline_Funcs* get_ft_outline_funcs()
{
    FT_Outline_Funcs* funs;
    SEXP ext_ptr = get_var_from_pkg_env(".outline_funs", "showtext");
    funs = (FT_Outline_Funcs*) R_ExternalPtrAddr(ext_ptr);

    return funs;
}

int get_num_segments()
{
    SEXP nseg = get_var_from_pkg_env(".nseg", "showtext");

    return INTEGER(nseg)[0];
}

double get_dev_units_per_point()
{
    SEXP dev_units_per_point = get_var_from_pkg_env(".dev_units_per_point", "showtext");

    return REAL(dev_units_per_point)[0];
}

pDevDesc get_saved_dev_desc()
{
    pDevDesc dd_save;
    SEXP ext_ptr = get_var_from_pkg_env(".dd_saved", "showtext");
    dd_save = (pDevDesc) R_ExternalPtrAddr(ext_ptr);

    return dd_save;
}

pGEDevDesc get_saved_device_id()
{
    pGEDevDesc gdd_saved;
    SEXP ext_ptr = get_var_from_pkg_env(".device_id", "showtext");
    gdd_saved = (pGEDevDesc) R_ExternalPtrAddr(ext_ptr);

    return gdd_saved;
}

Rboolean use_raster()
{
    SEXP use_raster = get_var_from_pkg_env(".use_raster", "showtext");

    return LOGICAL(use_raster)[0];
}
