#ifndef RASTER_H_INCLUDED
#define RASTER_H_INCLUDED

#include "fonts.h"

typedef struct rasterData {
    int nrow;
    int ncol;
    unsigned int *data;  /* by row */
} RasterData;

RasterData* RasterData_new(int nrow, int ncol);
void        RasterData_destroy(RasterData* rd);

RasterData* get_string_raster_image(unsigned int* unicode, int nchar,
                                    int px_size_x, int px_size_y,
                                    double rot, double hadj, const pGEcontext gc,
                                    double* center_horiz, double* center_vert);


#endif /* RASTER_H_INCLUDED */
