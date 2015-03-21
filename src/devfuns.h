#ifndef DEVFUNS_H_INCLUDED
#define DEVFUNS_H_INCLUDED

#include "fonts.h"


void showtextMetricInfo(int c, const pGEcontext gc, double* ascent,
    double* descent, double* width, pDevDesc dd);
    
double showtextStrWidthUTF8(const char *str, const pGEcontext gc, pDevDesc dd);

void showtextTextUTF8Raster(double x, double y, const char *str,
    double rot, double hadj, const pGEcontext gc, pDevDesc dd);

void showtextTextUTF8Polygon(double x, double y, const char *str,
    double rot, double hadj, const pGEcontext gc, pDevDesc dd);


#endif /* DEVFUNS_H_INCLUDED */
