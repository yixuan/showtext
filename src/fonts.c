#include "fonts.h"
#include "utf8.h"
#include "util.h"


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

/* Errors that may occur in loading font characters.
   Here we just give warnings. */
void FTError(FT_Error err)
{
    switch(err)
    {
        case 0x10:
            Rf_warning("freetype: invalid glyph index");
            break;
        case 0x11:
            Rf_warning("freetype: invalid character code");
            break;
        case 0x12:
            Rf_warning("freetype: unsupported glyph image format");
            break;
        case 0x13:
            Rf_warning("freetype: cannot render this glyph format");
            break;
        case 0x14:
            Rf_warning("freetype: invalid outline");
            break;
        case 0x15:
            Rf_warning("freetype: invalid composite glyph");
            break;
        case 0x16:
            Rf_warning("freetype: too many hints");
            break;
        case 0x17:
            Rf_warning("freetype: invalid pixel size");
            break;
        default:
            Rf_warning("freetype: error code %d", err);
            break;
    }
}

/* bearingY is the vertical distance from the baseline to the top of the glyph
   tail is the vertical distance from the baseline to the bottom of the glyph
   Both are positive integers */
void GetCharMetrics(FT_Face face, unsigned int ch,
                    int *bearingY, int *tail, int *advance)
{
    FT_Error err = FT_Load_Char(face, ch, FT_LOAD_RENDER);
    if(err)  FTError(err);
    *bearingY = face->glyph->bitmap_top;
    *tail = face->glyph->bitmap.rows - *bearingY;
    *advance = face->glyph->advance.x / 64;
}

void GetStringMetrics(FT_Face face, const unsigned int *str, int nchar,
                      int *bearingY, int *tail, int *advance)
{
    int charBearingY, charTail, charAdvance;
    *bearingY = 0;
    *tail = 0;
    *advance = 0;

    for(int i = 0; i < nchar; i++)
    {
        GetCharMetrics(face, str[i], &charBearingY, &charTail, &charAdvance);
        if(charBearingY > *bearingY)  *bearingY = charBearingY;
        if(charTail > *tail)  *tail = charTail;
        *advance += charAdvance;
    }
}
