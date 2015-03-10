#ifndef OUTLINE_H_INCLUDED
#define OUTLINE_H_INCLUDED

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#define R_USE_PROTOTYPES 1
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "array.h"

// FreeType will draw characters around the origin (0, 0).
// We need to "rotate" the character by some angle theta,
// and "move" the origin to (x, y).
// sign is used to identify the direction of y axis in the device.
// sign = 1 if
//      ^
//      |
//      |
//      |
// (0,0)|---------->
//
// sign = -1 if
// (0,0)|---------->
//      |
//      |
//      |
//      v
typedef struct {
    double x;
    double y;
    double theta; // counterclockwise about bottomleft corner
    int sign;
} TransData;

// Simple structure to represent point
typedef struct {
    double x;
    double y;
} Point;

// These are data passed to outline drawing handlers
typedef struct
{
    double ratio_EM; 
    double deltax;
    int nseg; // number of segments in drawin bezier curves
    Point curr_dev_trans; // current position
    TransData trans; // transformation data
    Array outline_x;
    Array outline_y;
    int npoly;
    int nper[200];
} OutlineData;


// transform point, i.e., rotate by theta and translate by (x, y)
void transformPoint(Point *before, Point *after, TransData *trans);

// Outline drawing functions used by FT_Outline_Decompose()
int outlineMoveTo(const FT_Vector* to, void* user);
int outlineLineTo(const FT_Vector* to, void* user);
int outlineConicTo(const FT_Vector* control, const FT_Vector* to, void* user);
int outlineCubicTo(const FT_Vector* control1, const FT_Vector* control2,
                   const FT_Vector* to, void* user);

// Warning/Error message of FreeType
void errorcode(FT_Error err);

#endif /* OUTLINE_H_INCLUDED */
