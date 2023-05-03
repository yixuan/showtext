#include "util.h"

SEXP get_var_from_env(const char* var_name, SEXP env)
{
    SEXP var = PROTECT(Rf_findVar(Rf_install(var_name), env));
    if(var == R_UnboundValue)
        Rf_error("variable '%s' not found", var_name);
    if(TYPEOF(var) == PROMSXP)
    {
        var = Rf_eval(var, env);
        /* Unprotect the old var */
        UNPROTECT(1);
        /* Protect the new var */
        PROTECT(var);
    }
    UNPROTECT(1);

    return var;
}

SEXP get_pkg_env(const char* pkg_name)
{
    /* See grDevices/src/devPS getFontDB() */
    SEXP pkg_namespace, pkg_env;
    pkg_namespace = PROTECT(R_FindNamespace(Rf_mkString(pkg_name)));
    pkg_env = PROTECT(get_var_from_env(".pkg.env", pkg_namespace));
    UNPROTECT(2);

    return pkg_env;
}

SEXP get_var_from_pkg_env(const char* var_name, const char* pkg_name)
{
    SEXP pkg_env, var;

    pkg_env = PROTECT(get_pkg_env(pkg_name));
    var = PROTECT(get_var_from_env(var_name, pkg_env));
    UNPROTECT(2);

    return var;
}

FT_Outline_Funcs* get_ft_outline_funcs(void)
{
    FT_Outline_Funcs* funs;
    SEXP ext_ptr = PROTECT(get_var_from_pkg_env(".outline_funs", "showtext"));
    funs = (FT_Outline_Funcs*) R_ExternalPtrAddr(ext_ptr);
    UNPROTECT(1);

    return funs;
}

int get_num_segments(void)
{
    SEXP nseg = PROTECT(get_var_from_pkg_env(".nseg", "showtext"));
    int res = INTEGER(nseg)[0];
    UNPROTECT(1);

    return res;
}

void get_device_id(pGEDevDesc gdd, char* id)
{
    /* We take the pointer address of gdd as the ID */
    char pointer_str[20];
    strcpy(id, "dev_");
    snprintf(pointer_str, 20, "%p", (void*) gdd);
    strncat(id, pointer_str, 20);
}

SEXP get_device_data(pGEDevDesc gdd)
{
    SEXP devs_env, dev_data;
    char dev_id[50];
    get_device_id(gdd, dev_id);

    devs_env = PROTECT(get_var_from_pkg_env(".devs", "showtext"));
    dev_data = PROTECT(get_var_from_env(dev_id, devs_env));
    UNPROTECT(2);

    return dev_data;
}
