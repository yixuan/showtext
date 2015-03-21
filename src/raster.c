#include "raster.h"


RasterData* NewRasterData(int nrow, int ncol)
{
    RasterData *rd = (RasterData *) malloc(sizeof(RasterData));
    rd->nrow = nrow;
    rd->ncol = ncol;
    rd->data = (unsigned int *) calloc(nrow * ncol, sizeof(unsigned int));
    
    return rd;
}

void FreeRasterData(RasterData *rd)
{
    if(rd && rd->data)  free(rd->data);
    if(rd)  free(rd);
}



/* Write bitmap to the matrix
   The upper-left corner of the bitmap will be written to matrix(i, j),
   i.e., matrix(i + s, j + t) = buffer(s, t) */
static void WriteMatrix(const FT_Bitmap *bitmap, RasterData *mat, int mi, int mj,
                        const pGEcontext gc)
{
    /* Bitmap is stored by row, starting from the upperleft corner.
       Therefore, the pixel in the p-th row and q-th column (0-based) is
       buffer[p * bitmap->pitch + q] */
    int bm_width = bitmap->width, bm_height = bitmap->rows;
    int mat_ncol = mat->ncol, mat_nrow = mat->nrow;
    int i, j, p, q;

    for(i = mi, p = 0; p < bm_height; i++, p++)
    {
        for(j = mj, q = 0; q < bm_width; j++, q++)
        {
            if(i < 0 || j < 0 || i >= mat_nrow || j >= mat_ncol)
                continue;

            unsigned char intensity = bitmap->buffer[p * bitmap->pitch + q];
            /* RasterData is also stored by row */
            mat->data[i * mat_ncol + j] = R_RGBA(R_RED(gc->col),
                                                 R_GREEN(gc->col),
                                                 R_BLUE(gc->col),
                                                 (R_ALPHA(gc->col) * intensity) / 255);
        }
    }
}

RasterData* GetStringRasterImage(unsigned int *unicode, int nchar,
                                 int psizeX, int psizeY, const pGEcontext gc)
{
    FT_Face face = GetFTFace(gc);
    FT_GlyphSlot slot = face->glyph;
    FT_Error err;
    
    /* How many rows above baseline */
    int bearingY;
    /* How many rows below baseline */
    int tail;
    /* How many columns */
    int advance;
    /* Enty to be written */
    int mi = 0, mj = 0, i;
    
    /* Raster data */
    RasterData* rd;
    
    /* Set pixel size */
    err = FT_Set_Pixel_Sizes(face, psizeX, psizeY);
    if(err)  FTError(err);

    /* Get metric information */
    GetStringMetrics(face, unicode, nchar, &bearingY, &tail, &advance);
    
    /* Create raster data */
    rd = NewRasterData(bearingY + tail, advance);
    for(i = 0; i < nchar; i++)
    {
        FT_Load_Char(face, unicode[i], FT_LOAD_RENDER);
        mi = bearingY - slot->bitmap_top;
        WriteMatrix(&(slot->bitmap), rd, mi, mj + slot->bitmap_left, gc);
        mj += slot->advance.x / 64;
    }
    
    return rd;
}
