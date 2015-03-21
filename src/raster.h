#ifndef RASTER_H_INCLUDED
#define RASTER_H_INCLUDED

#include "fonts.h"

typedef struct rasterData {
    int nrow;
    int ncol;
    unsigned int *data;
} RasterData;

RasterData* NewRasterData(int nrow, int ncol);
void FreeRasterData(RasterData *rd);

RasterData* GetStringRasterImage(unsigned int *unicode, int nchar,
                                 int psizeX, int psizeY,
                                 double rot, double hadj, const pGEcontext gc,
                                 double *centerH, double *centerV);


#endif /* RASTER_H_INCLUDED */
