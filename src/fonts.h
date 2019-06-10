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
    FT_Face    face;
} FontDesc;

/* Convert UTF-8 string to Unicode array
 * utf8: input UTF-8 string
 * n:    length of UTF-8 string (bytes)
 * ucs4: output array of decoded characters
 *
 * Returns the length of output array
 */
int utf8_to_ucs4(unsigned int* ucs4, const char* utf8, int n);

/* Test whether all characters in the array are smaller than 1024 in Unicode.
 * If yes, we use "sans" as the default font family in get_ft_face();
 * otherwise choose "wqy-microhei".
 */
int all_smaller_than_1024(unsigned int* unicode, int nchar);

/* Get FreeType font face object from R graphics parameters */
FT_Face get_ft_face(const pGEcontext gc, const char* default_family);

/* Forward FreeType warning/error message to R */
void forward_ft_error(FT_Error err);

/* Get the bounding box of a string */
void get_string_bbox(FT_Face face, const unsigned int* str, int nchar, double rot,
                     int* xmin, int* xmax, int* ymin, int* ymax);


#endif /* FONTS_H_INCLUDED */

