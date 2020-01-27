#include "raster.h"


RasterData* RasterData_new(int nrow, int ncol)
{
    RasterData* rd = (RasterData*) malloc(sizeof(RasterData));
    int i;
    
    rd->nrow = nrow;
    rd->ncol = ncol;
    rd->data = (unsigned int *) calloc(nrow * ncol, sizeof(unsigned int));
    /* Fill data with transparent white */
    for(i = 0; i < nrow * ncol; i++)
        rd->data[i] = R_TRANWHITE;
    
    return rd;
}

void RasterData_destroy(RasterData* rd)
{
    if(!rd) return;
    if(rd->data)  free(rd->data);
    if(rd)        free(rd);
}



/* Write bitmap to the matrix
   The upper-left corner of the bitmap will be written to matrix(i, j),
   i.e., matrix(i + s, j + t) = buffer(s, t) */
static void write_matrix(const FT_Bitmap* bitmap, RasterData* mat, int mi, int mj,
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
            if(intensity > 0)
                mat->data[i * mat_ncol + j] = R_RGBA(
                    R_RED(gc->col),
                    R_GREEN(gc->col),
                    R_BLUE(gc->col),
                    /* Reference: https://lists.ffmpeg.org/pipermail/ffmpeg-cvslog/2015-December/096038.html */
                    (unsigned)((R_ALPHA(gc->col) * intensity) / 255)
                );
        }
    }
}

RasterData* get_string_raster_image(unsigned int* unicode, int nchar,
                                    int px_size_x, int px_size_y,
                                    double rad, double hadj, const pGEcontext gc,
                                    double* center_horiz, double* center_vert)
{
    FT_Face face = get_ft_face(gc, all_smaller_than_1024(unicode, nchar) ? "sans" : "wqy-microhei");
    FT_GlyphSlot slot = face->glyph;
    FT_Matrix trans;
    FT_Vector pen;
    FT_Error err;
    
    /* String bounding box, in Cartesian system */
    int xmin, xmax, ymin, ymax;
    /* Alignment center, in Cartesian system */
    double cx, cy;
    /* Rotation coefficients */
    double sinr = sin(rad), cosr = cos(rad);
    /* Raster data */
    RasterData* rd;
    /* Entry point of the raster data to be written to */
    int mi = 0, mj = 0, i;
    
    /* Set pixel size */
    err = FT_Set_Pixel_Sizes(face, px_size_x, px_size_y);
    if(err)  forward_ft_error(err);

    /* Get bounding box when no transformation is applied */
    get_string_bbox(face, unicode, nchar, 0.0, &xmin, &xmax, &ymin, &ymax);
    /* Calculate the alignment center before transformation */
    cx = hadj * xmax + (1 - hadj) * xmin;
    cy = 0.0;
    /* Obtain the center after transformation */
    /* x' = x * cos(r) - y * sin(r) */
    /* y' = x * sin(r) + y * cos(r) */
    cy = cx * sinr;
    cx = cx * cosr;
    
    /* Calculate bounding box when transformation is applied */
    get_string_bbox(face, unicode, nchar, rad, &xmin, &xmax, &ymin, &ymax);
    /* Calculate the horizontal and vertical distance from the alignment center
       to the bottom-left corner of the (new) bounding box */
    *center_horiz = cx - xmin;
    *center_vert = cy - ymin;
    
    /* Set transformation */
    trans.xx = trans.yy = (FT_Fixed)(cosr * 0x10000L);
    trans.xy = (FT_Fixed)(-sinr * 0x10000L);
    trans.yx = -trans.xy;
    pen.x = pen.y = 0;
    
    /* Create raster data */
    rd = RasterData_new(ymax - ymin, xmax - xmin);
    for(i = 0; i < nchar; i++)
    {
        FT_Set_Transform(face, &trans, &pen);
        
        /* There is an issue about FT_LOAD_RENDER, see
         * https://github.com/yixuan/showtext/issues/29.
         * I guess it is due to the bitmap glyph contained in the MS Gothic font,
         * so we force not to load the bitmap glyph.
         */
        /* FT_Load_Char(face, unicode[i], FT_LOAD_RENDER); */
        FT_Load_Char(face, unicode[i], FT_LOAD_NO_BITMAP);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

        mi = ymax - slot->bitmap_top;
        mj = slot->bitmap_left - xmin;
        write_matrix(&(slot->bitmap), rd, mi, mj, gc);
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }
    
    /* Restore to identity */
    trans.xx = trans.yy = 0x10000L;
    trans.xy = trans.yx = 0;
    pen.x = pen.y = 0;
    FT_Set_Transform(face, &trans, &pen);
    
    return rd;
}
