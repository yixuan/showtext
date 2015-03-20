#ifndef FONTS_H_INCLUDED
#define FONTS_H_INCLUDED

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#ifndef R_USE_PROTOTYPES
  #define R_USE_PROTOTYPES 1
#endif
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

typedef struct fontDesc {
    FT_Library library;
    FT_Face face;
} FontDesc;

/* Convert UTF-8 string to Unicode array */
int utf8toucs4(unsigned int *ucs4, const char *utf8, int n);

/* Get FreeType font face from description in R */
FT_Face GetFTFace(const pGEcontext gc);

/* Warning/Error message of FreeType */
void FTError(FT_Error err);

/* Get the font metrics of a character */
void GetCharMetrics(FT_Face face, unsigned int ch,
                    int *bearingY, int *tail, int *advance);

/* Get the font metrics of a string */
void GetStringMetrics(FT_Face face, const unsigned int *str, int nchar,
                      int *bearingY, int *tail, int *advance);


#endif /* FONTS_H_INCLUDED */

