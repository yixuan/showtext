#ifndef OUTLINE_H_INCLUDED
#define OUTLINE_H_INCLUDED

#include "fonts.h"
#include "array.h"

/* FreeType will draw characters around the origin (0, 0).
   We need to "rotate" the character by some angle theta,
   and "move" the origin to (x, y).
   sign is used to identify the direction of y axis in the device.
   sign = 1 if
        ^
        |
        |
        |
   (0,0)|---------->
  
   sign = -1 if
   (0,0)|---------->
        |
        |
        |
        v
*/
typedef struct {
    double x;
    double y;
    double theta; /* counterclockwise about bottomleft corner */
    int    sign;
} TransData;

/* Simple structure to represent a point in the device */
typedef struct {
    double x;
    double y;
} Point;

/* These are data passed to outline drawing handlers */
typedef struct {
    double    ft_to_dev_ratio;  /* FT_Vector_coord * ratio == Device_coord */
    double    offset_x;         /* distance from the origin of the string to the
                                   current character */
    int       num_segments;     /* number of segments in drawing bezier curves */
    Point     curr_pos;         /* current position of the pen */
    TransData trans;            /* transformation data */
    Array     outline_x;        /* array to record the x coordinates of the outline */
    Array     outline_y;        /* array to record the y coordinates of the outline */
    int       num_poly;         /* number of polygons in the outline */
    IntArray  points_in_poly;   /* number of points in each polygon */
} OutlineData;


/* Transform point, i.e., rotate by theta and translate by (x, y) */
void transform_point(Point* before, Point* after, TransData* trans);

/* Outline drawing functions used by FT_Outline_Decompose() */
int outline_move_to (const FT_Vector* to, void* user);
int outline_line_to (const FT_Vector* to, void* user);
int outline_conic_to(const FT_Vector* control, const FT_Vector* to, void* user);
int outline_cubic_to(const FT_Vector* control1, const FT_Vector* control2,
                     const FT_Vector* to, void* user);

/* Functions to be exported to R */
SEXP showtext_new_outline_funs(void);


#endif /* OUTLINE_H_INCLUDED */
