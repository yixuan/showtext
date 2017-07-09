#ifndef DEVFUNS_H_INCLUDED
#define DEVFUNS_H_INCLUDED

#include "fonts.h"


void showtext_metric_info(int c, const pGEcontext gc, double* ascent,
    double* descent, double* width, pDevDesc dd);
    
double showtext_str_width_utf8(const char* str, const pGEcontext gc, pDevDesc dd);

void showtext_text_utf8_raster(double x, double y, const char* str,
    double rot, double hadj, const pGEcontext gc, pDevDesc dd);

void showtext_text_utf8_polygon(double x, double y, const char* str,
    double rot, double hadj, const pGEcontext gc, pDevDesc dd);


#endif /* DEVFUNS_H_INCLUDED */
