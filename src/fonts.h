#ifndef FONTS_H_INCLUDED
#define FONTS_H_INCLUDED

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct fontDesc {
    FT_Library library;
    FT_Face face;
} FontDesc;

#endif /* FONTS_H_INCLUDED */

