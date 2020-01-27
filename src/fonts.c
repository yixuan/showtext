#include "fonts.h"
#include "utf8.h"
#include "util.h"


/* Convert a UTF-8 string to an array of Unicodes */
int utf8_to_ucs4(unsigned int* ucs4, const char* utf8, int n)
{
    int len = 0;
    int step = 0;
    int err;
    unsigned int* p1;
    const char* p2;
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

/* A small example */
/*
SEXP utf8_to_int(SEXP str)
{
    const char* s = CHAR(STRING_ELT(str, 0));
    int maxlen = strlen(s);
    unsigned int* buf = calloc(maxlen + 1, sizeof(int));
    int len = utf8_to_ucs4(buf, s, maxlen);
    SEXP res;
    int i;
    PROTECT(res = allocVector(INTSXP, len));
    for(i = 0; i < len; i++)
        INTEGER(res)[i] = buf[i];
    free(buf);
 
    UNPROTECT(1);
    return res;
}
*/



/* Test whether all characters in the array are smaller than 1024 in Unicode */
int all_smaller_than_1024(unsigned int* unicode, int nchar)
{
    int i;
    for(i = 0; i < nchar; i++)
    {
        if(unicode[i] >= 1024)
            return 0;
    }
    return 1;
}

/* Obtain the FT_Face structure given family name and font face */
FT_Face get_ft_face(const pGEcontext gc, const char* default_family)
{
    int font_face = gc->fontface;
    FontDesc* font;
    
    SEXP font_list;
    SEXP font_names;
    SEXP ext_ptr;
    int i, list_len;
    
    /* Font list is sysfonts:::.pkg.env$.font.list,
       defined in sysfonts/R/font.R */    
    font_list = PROTECT(get_var_from_pkg_env(".font.list", "sysfonts"));
    
    /* Search the given family name */
    font_names = PROTECT(GET_NAMES(font_list));
    list_len = Rf_length(font_list);
    for(i = 0; i < list_len; i++)
    {
        if(strcmp(gc->fontfamily, CHAR(STRING_ELT(font_names, i))) == 0)
        {
            break;
        }
    }
    /* If not found, search the default family */
    if(i == list_len)
    {
        if(gc->fontfamily[0] != '\0')
            Rf_warning("font family '%s' not found, will use '%s' instead", gc->fontfamily, default_family);
        for(i = 0; i < list_len; i++)
        {
            if(strcmp(default_family, CHAR(STRING_ELT(font_names, i))) == 0)
            {
                break;
            }
        }
    }
    /* If still not found, use "sans" */
    if(i == list_len)
    {
        Rf_warning("font family '%s' not found, will use 'sans' instead", default_family);
        i = 0;
    }
    if(font_face < 1 || font_face > 5) font_face = 1;
    
    ext_ptr = VECTOR_ELT(VECTOR_ELT(font_list, i), font_face - 1);
    font = (FontDesc*) R_ExternalPtrAddr(ext_ptr);
    
    UNPROTECT(2);
    return font->face;
}

/* Errors that may occur in loading font characters.
   Here we just give warnings. */
void forward_ft_error(FT_Error err)
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

/* Get the bounding box of a string, with a possible rotation */
void get_string_bbox(FT_Face face, const unsigned int* str, int nchar, double rot,
                     int* xmin, int* xmax, int* ymin, int* ymax)
{
    int char_xmin, char_xmax, char_ymin, char_ymax;
    FT_GlyphSlot slot = face->glyph;
    FT_Matrix trans;
    FT_Vector pen;
    FT_Error err;
    int i;
    
    *xmin = *xmax = *ymin = *ymax = 0;
    
    /* Set rotation transformation */
    trans.xx = trans.yy = (FT_Fixed)( cos(rot) * 0x10000L);
    trans.xy = (FT_Fixed)(-sin(rot) * 0x10000L);
    trans.yx = -trans.xy;
    pen.x = pen.y = 0;

    for(i = 0; i < nchar; i++)
    {
        FT_Set_Transform(face, &trans, &pen);

        /* See raster.c for the explanation */
        /* err = FT_Load_Char(face, str[i], FT_LOAD_RENDER); */
        err = FT_Load_Char(face, str[i], FT_LOAD_NO_BITMAP);
        if(err)  forward_ft_error(err);
        err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if(err)  forward_ft_error(err);
        
        char_xmin = slot->bitmap_left;
        char_xmax = char_xmin + slot->bitmap.width;
        char_ymax = slot->bitmap_top;
        char_ymin = char_ymax - slot->bitmap.rows;
        if(i == 0)
        {
            *xmin = char_xmin;
            *xmax = char_xmax;
            *ymin = char_ymin;
            *ymax = char_ymax;
        } else {
            *xmin = char_xmin < *xmin ? char_xmin : *xmin;
            *xmax = char_xmax > *xmax ? char_xmax : *xmax;
            *ymin = char_ymin < *ymin ? char_ymin : *ymin;
            *ymax = char_ymax > *ymax ? char_ymax : *ymax;
        }
        /* Move the pen for the next character */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }
    
    /* Identity transformation */
    trans.xx = trans.yy = 0x10000L;
    trans.xy = trans.yx = 0;
    pen.x = pen.y = 0;
    /* Restore to identity */
    FT_Set_Transform(face, &trans, &pen);
}
