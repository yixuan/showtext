#include "fonts.h"
#include "utf8.h"

int utf8toucs4(unsigned int *ucs4, const char *utf8, int n)
{
    int len = 0;
    int step = 0;
    int err;
    unsigned int *p1;
    const char * p2;
    for(p1 = ucs4, p2 = utf8; ; p1++, p2 += step)
    {
        /* if we meet '\0' */
        if(!p2[0]) break;
        err = TY_(DecodeUTF8BytesToChar)(p1, p2[0], p2 + 1, &step);
        if(err)
        {
            Rf_warning("UTF-8 decoding error for '%s'", utf8);
            *p1 = 0xFFFD; /* replacement char */
        }
        len++;
        if(len >= n) break;
    }
    return len;
}

/* a small example */
/*
SEXP utf8toint(SEXP str)
{
    const char *s = CHAR(STRING_ELT(str, 0));
    int maxlen = strlen(s);
    unsigned int *buf = calloc(maxlen + 1, sizeof(int));
    int len = utf8toucs4(buf, s, maxlen);
    SEXP res;
    int i;
    PROTECT(res = allocVector(INTSXP, len));
    for(i = 0; i < len; i++)
        INTEGER(res)[i] = buf[i];
    UNPROTECT(1);
    free(buf);
    return res;
}
*/



FT_Face GetFTFace(const pGEcontext gc)
{
    int gcfontface = gc->fontface;
    FontDesc *font;
    
    SEXP fontList;
    SEXP fontNames;
    SEXP extPtr;
    int i, listLen;
    
    /* Font list is sysfonts:::.pkg.env$.font.list,
       defined in sysfonts/R/font.R */    
    fontList = GetVarFromPkgEnv(".font.list", "sysfonts");
    
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
    /* If not found, search "wqy-microhei" */
    if(i == listLen)
    {
        for(i = 0; i < listLen; i++)
        {
            if(strcmp("wqy-microhei", CHAR(STRING_ELT(fontNames, i))) == 0)
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
