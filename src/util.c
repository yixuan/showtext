#include "fonts.h"
#include "util.h"

size_t utf8toucs(wchar_t *wc, const char *s)
{
    unsigned int byte;
    wchar_t local, *w;
    byte = *((unsigned char *)s);
    w = wc ? wc: &local;

    if (byte == 0)
    {
        *w = (wchar_t) 0;
        return 0;
    }
    else if (byte < 0xC0)
    {
        *w = (wchar_t) byte;
        return 1;
    }
    else if (byte < 0xE0)
    {
        if(strlen(s) < 2) return (size_t)-2;
        if ((s[1] & 0xC0) == 0x80)
        {
            *w = (wchar_t) (((byte & 0x1F) << 6) | (s[1] & 0x3F));
            return 2;
        }
        else return (size_t)-1;
    }
    else if (byte < 0xF0)
    {
        if(strlen(s) < 3) return (size_t)-2;
        if (((s[1] & 0xC0) == 0x80) && ((s[2] & 0xC0) == 0x80))
        {
            *w = (wchar_t) (((byte & 0x0F) << 12)
                            | (unsigned int) ((s[1] & 0x3F) << 6)
                            | (s[2] & 0x3F));
            byte = (unsigned int) *w;
            /* Surrogates range */
            if(byte >= 0xD800 && byte <= 0xDFFF) return (size_t)-1;
            if(byte == 0xFFFE || byte == 0xFFFF) return (size_t)-1;
            return 3;
        }
        else return (size_t)-1;
    }
    if(sizeof(wchar_t) < 4) return (size_t)-2;
    /* So now handle 4,5.6 byte sequences with no testing */
    if (byte < 0xf8)
    {
        if(strlen(s) < 4) return (size_t)-2;
        *w = (wchar_t) (((byte & 0x0F) << 18)
                        | (unsigned int) ((s[1] & 0x3F) << 12)
                        | (unsigned int) ((s[2] & 0x3F) << 6)
                        | (s[3] & 0x3F));
        return 4;
    }
    else if (byte < 0xFC)
    {
        if(strlen(s) < 5) return (size_t)-2;
        *w = (wchar_t) (((byte & 0x0F) << 24)
                        | (unsigned int) ((s[1] & 0x3F) << 12)
                        | (unsigned int) ((s[2] & 0x3F) << 12)
                        | (unsigned int) ((s[3] & 0x3F) << 6)
                        | (s[4] & 0x3F));
        return 5;
    }
    else
    {
        if(strlen(s) < 6) return (size_t)-2;
        *w = (wchar_t) (((byte & 0x0F) << 30)
                        | (unsigned int) ((s[1] & 0x3F) << 24)
                        | (unsigned int) ((s[2] & 0x3F) << 18)
                        | (unsigned int) ((s[3] & 0x3F) << 12)
                        | (unsigned int) ((s[4] & 0x3F) << 6)
                        | (s[5] & 0x3F));
        return 6;
    }
}

int utf8towcs(wchar_t *wc, const char *s, int n)
{
    ssize_t m, res = 0;
    const char *t;
    wchar_t *p;
    wchar_t local;

    if(wc)
        for(p = wc, t = s; ; p++, t += m)
        {
            m  = (ssize_t) utf8toucs(p, t);
            if (m < 0) Rf_error("invalid input '%s' in 'utf8towcs'", s);
            if (m == 0) break;
            res ++;
            if (res >= n) break;
        }
    else
        for(t = s; ; res++, t += m)
        {
            m  = (ssize_t) utf8toucs(&local, t);
            if (m < 0) Rf_error("invalid input '%s' in 'utf8towcs'", s);
            if (m == 0) break;
        }
    return (int) res;
}

static SEXP GetVarFromPkgEnv(const char *varName, const char *pkgName)
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

FT_Face GetFTFace(const pGEcontext gc)
{
    int gcfontface = gc->fontface;
    FontDesc *font;
    
    SEXP fontList;
    SEXP fontNames;
    SEXP extPtr;
    int i, listLen;
    
    /* Font list is R2SWF:::.pkg.env$.font.list,
       defined in R2SWF/R/font.R */    
    fontList = GetVarFromPkgEnv(".font.list", "R2SWF");
    
    /* Search the given family name */
    fontNames = GET_NAMES(fontList);
    listLen = Rf_length(fontList);
    for(i = 0; i < listLen; i++)
    {
        if(strcmp(gc->fontfamily, CHAR(STRING_ELT(fontNames, i))) == 0)
        {
            break;
        }
    }
    /* If not found, search "wqy" */
    if(i == listLen)
    {
        for(i = 0; i < listLen; i++)
        {
            if(strcmp("wqy", CHAR(STRING_ELT(fontNames, i))) == 0)
            {
                break;
            }
        }
    }
    /* If still not found, use "sans" */
    if(i == listLen) i = 0;
    if(gcfontface < 1 || gcfontface > 5) gcfontface = 1;
    
    extPtr = VECTOR_ELT(VECTOR_ELT(fontList, i), gcfontface - 1);
    font = (FontDesc *) R_ExternalPtrAddr(extPtr);
    
    return font->face;
}

FT_Outline_Funcs* GetFTOutlineFuncs()
{
    FT_Outline_Funcs *funs;
    SEXP extPtr;

    extPtr = GetVarFromPkgEnv(".outline.funs", "showtext");
    funs = (FT_Outline_Funcs *) R_ExternalPtrAddr(extPtr);

    return funs;
}

int GetNseg()
{
    SEXP nseg;
    
    nseg = GetVarFromPkgEnv(".nseg", "showtext");

    return INTEGER(nseg)[0];
}

pDevDesc GetDevDesc()
{
    pDevDesc dd_save;
    SEXP extPtr;

    extPtr = GetVarFromPkgEnv(".dd.save", "showtext");
    dd_save = (pDevDesc) R_ExternalPtrAddr(extPtr);

    return dd_save;
}

pGEDevDesc GetGEDevDesc()
{
    pGEDevDesc gdd_save;
    SEXP extPtr;

    extPtr = GetVarFromPkgEnv(".gdd.save", "showtext");
    gdd_save = (pGEDevDesc) R_ExternalPtrAddr(extPtr);

    return gdd_save;
}

